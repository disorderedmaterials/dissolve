// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/forcekernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>

ForceKernel::ForceKernel(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                         std::optional<double> energyCutoff)
    : box_(cfg->box()), cellArray_(cfg->cells()), molecules_(cfg->molecules()), potentialMap_(potentialMap),
      processPool_(procPool)
{
    cutoffDistanceSquared_ =
        energyCutoff.has_value() ? energyCutoff.value() * energyCutoff.value() : potentialMap_.range() * potentialMap_.range();
}

ForceKernel::ForceKernel(const ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap,
                         std::optional<double> energyCutoff)
    : box_(box), potentialMap_(potentialMap), processPool_(procPool)
{
    cutoffDistanceSquared_ =
        energyCutoff.has_value() ? energyCutoff.value() * energyCutoff.value() : potentialMap_.range() * potentialMap_.range();
}

/*
 * Force Calculation
 */

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f) const
{
    auto force = j.r() - i.r();
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r);
    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f, double elecScale, double vdwScale) const
{
    auto force = j.r() - i.r();
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r, elecScale, vdwScale);
    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithMim(const Atom &i, const Atom &j, ForceVector &f) const
{
    auto force = box_->minimumVector(i.r(), j.r());
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r);

    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithMim(const Atom &i, const Atom &j, ForceVector &f, double elecScale, double vdwScale) const
{
    auto force = box_->minimumVector(i.r(), j.r());
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r, elecScale, vdwScale);

    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms in supplied cells
void ForceKernel::forces(const Cell *centralCell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                         ProcessPool::DivisionStrategy strategy, ForceVector &f) const
{
    assert(centralCell && otherCell);
    auto &centralAtoms = centralCell->atoms();
    auto &otherAtoms = otherCell->atoms();
    Vec3<double> rI;
    std::shared_ptr<Molecule> molI;

    // Get start/stride for specified loop context
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    // Loop over central cell atoms
    if (applyMim)
    {
        auto [begin, end] = chop_range(centralAtoms.begin(), centralAtoms.end(), nChunks, offset);
        for (auto indexI = begin; indexI < end; ++indexI)
        {
            auto &ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto *jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii->arrayIndex() >= jj->arrayIndex()))
                    continue;

                // Check for atoms in the same Molecule
                if (molI != jj->molecule())
                    forcesWithMim(*ii, *jj, f);
                else
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        forcesWithMim(*ii, *jj, f);
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        forcesWithMim(*ii, *jj, f, elec14, vdw14);
                }
            }
        }
    }
    else
    {
        auto [begin, end] = chop_range(centralCell->atoms().begin(), centralCell->atoms().end(), nChunks, offset);
        for (auto indexI = begin; indexI < end; ++indexI)
        {
            auto &ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto &jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii->arrayIndex() >= jj->arrayIndex()))
                    continue;

                // Check for atoms in the same molecule
                if (molI != jj->molecule())
                    forcesWithoutMim(*ii, *jj, f);
                else
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        forcesWithoutMim(*ii, *jj, f);
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        forcesWithoutMim(*ii, *jj, f, elec14, vdw14);
                }
            }
        }
    }
}

// Calculate forces between Cell and its neighbours
void ForceKernel::forces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy, ForceVector &f) const
{
    assert(cellArray_);

    auto &neighbours = cellArray_->get().neighbours(*cell);
    for (auto it = std::next(neighbours.begin()); it != neighbours.end(); ++it)
        forces(cell, &it->neighbour_, it->requiresMIM_, excludeIgeJ, strategy, f);
}

// Calculate total pairpotential forces between atoms in the world
void ForceKernel::totalPairPotentialForces(std::vector<Vec3<double>> &f, ProcessPool::DivisionStrategy strategy) const
{
    assert(cellArray_);
    auto &cellArray = cellArray_->get();

    // Create a ForceKernel
    auto combinableForces = createCombinableForces(f);

    // Set start/stride for parallel loop
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

    // algorithm parameters
    auto unaryOp = [&, strategy](const int id)
    {
        auto *cellI = cellArray.cell(id);
        auto &fLocal = combinableForces.local();
        // This cell with itself
        forces(cellI, cellI, false, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
        // Interatomic interactions between atoms in this cell and its neighbours
        forces(cellI, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
    };

    // Execute lambda operator for each cell
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    combinableForces.finalize();
}

/*
 * Intramolecular Terms
 */

// Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index and input vector
void ForceKernel::addTorsionForceI(double du_dphi, int index, ForceKernel::TorsionParameters &torsionParameters,
                                   ForceVector &f) const
{
    auto &dcos_dxpj = torsionParameters.dcos_dxpj_;

    f[index].add(du_dphi * torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dij_.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dij_.columnAsVec3(1)),
                 du_dphi * dcos_dxpj.dp(torsionParameters.dxpj_dij_.columnAsVec3(2)));
}

// Add torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index and input vector
void ForceKernel::addTorsionForceJ(double du_dphi, int index, ForceKernel::TorsionParameters &torsionParameters,
                                   ForceVector &f) const
{
    f[index].add(du_dphi * (torsionParameters.dcos_dxpj_.dp(-torsionParameters.dxpj_dij_.columnAsVec3(0) -
                                                            torsionParameters.dxpj_dkj_.columnAsVec3(0)) -
                            torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(0))),
                 du_dphi * (torsionParameters.dcos_dxpj_.dp(-torsionParameters.dxpj_dij_.columnAsVec3(1) -
                                                            torsionParameters.dxpj_dkj_.columnAsVec3(1)) -
                            torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(1))),
                 du_dphi * (torsionParameters.dcos_dxpj_.dp(-torsionParameters.dxpj_dij_.columnAsVec3(2) -
                                                            torsionParameters.dxpj_dkj_.columnAsVec3(2)) -
                            torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(2))));
}

// Add torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index and input vector
void ForceKernel::addTorsionForceK(double du_dphi, int index, ForceKernel::TorsionParameters &torsionParameters,
                                   ForceVector &f) const
{
    f[index].add(du_dphi * (torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(0) -
                                                            torsionParameters.dxpk_dlk_.columnAsVec3(0)) +
                            torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dkj_.columnAsVec3(0))),
                 du_dphi * (torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(1) -
                                                            torsionParameters.dxpk_dlk_.columnAsVec3(1)) +
                            torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dkj_.columnAsVec3(1))),
                 du_dphi * (torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dkj_.columnAsVec3(2) -
                                                            torsionParameters.dxpk_dlk_.columnAsVec3(2)) +
                            torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dkj_.columnAsVec3(2))));
}

// Add torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index and input vector
void ForceKernel::addTorsionForceL(double du_dphi, int index, ForceKernel::TorsionParameters &torsionParameters,
                                   ForceVector &f) const
{
    f[index].add(du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(1)),
                 du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(2)));
}

// Calculate SpeciesBond forces
void ForceKernel::forces(const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    f[i.arrayIndex()] -= vecji;
    f[j.arrayIndex()] += vecji;
}

// Calculate SpeciesBond forces
void ForceKernel::forces(const SpeciesBond &bond, const Vec3<double> &ri, const Vec3<double> &rj, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);

    // Get distance and normalise vector ready for force calculation
    const auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    f[bond.i()->index()] -= vecji;
    f[bond.j()->index()] += vecji;
}

// Calculate angle force parameters from supplied vectors, storing results in supplied variables
ForceKernel::AngleParameters ForceKernel::calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk)
{
    // Calculate angle
    AngleParameters angleParameters;
    const auto magji = vecji.magAndNormalise();
    const auto magjk = vecjk.magAndNormalise();
    double dp;
    angleParameters.theta_ = Box::angleInDegrees(vecji, vecjk, dp);

    // Determine force vectors for atoms
    angleParameters.dfi_dtheta_ = (vecjk - vecji * dp) / magji;
    angleParameters.dfk_dtheta_ = (vecji - vecjk * dp) / magjk;

    return angleParameters;
}

// Calculate SpeciesAngle forces
void ForceKernel::forces(const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k, ForceVector &f) const
{
    auto vecji = box_->minimumVector(j.r(), i.r());
    auto vecjk = box_->minimumVector(j.r(), k.r());

    auto angleParameters = calculateAngleParameters(vecji, vecjk);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    f[i.arrayIndex()] += angleParameters.dfi_dtheta_;
    f[j.arrayIndex()] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    f[k.arrayIndex()] += angleParameters.dfk_dtheta_;
}

// Calculate SpeciesAngle forces
void ForceKernel::forces(const SpeciesAngle &angle, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                         ForceVector &f) const
{
    auto angleParameters = calculateAngleParameters(ri - rj, rk - rj);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    f[angle.i()->index()] += angleParameters.dfi_dtheta_;
    f[angle.j()->index()] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    f[angle.k()->index()] += angleParameters.dfk_dtheta_;
}

// Calculate torsion force parameters from supplied vectors, storing results in supplied variables
ForceKernel::TorsionParameters ForceKernel::calculateTorsionParameters(const Vec3<double> &vecji, const Vec3<double> &vecjk,
                                                                       const Vec3<double> &veckl)
{
    // Calculate cross products and torsion angle formed (in radians)
    auto xpj = vecji * vecjk;
    auto xpk = veckl * vecjk;
    const auto magxpj = xpj.magAndNormalise();
    const auto magxpk = xpk.magAndNormalise();
    auto dp = xpj.dp(xpk);

    /*
     * Construct derivatives of perpendicular axis (cross product) w.r.t. component vectors.
     * E.g.
     *	d (rij x rkj)
     *	------------- = rij[cp(n+2)] * U[cp(n+1)] - rij[cp(n+1)] * U[cp(n+2)]
     *	d rkj[n]
     *
     * where cp is a cyclic permutation spanning {0,1,2} == {x,y,z}, and U[n] is a unit vector in the n direction.
     * So,
     *	d (rij x rkj)
     *	------------- = rij[2] * U[1] - rij[1] * U[2]
     *	d rkj[0]
     *			= rij[z] * (0,1,0) - rij[y] * (0,0,1)
     *
     *			= (0,rij[z],0) - (0,0,rij[y])
     *
     *			= (0,rij[z],-rij[y])
     */

    TorsionParameters torsionParameters;
    torsionParameters.phi_ = atan2(vecjk.dp(xpj * xpk) / vecjk.magnitude(), dp);
    torsionParameters.dxpj_dij_.makeCrossProductMatrix(vecjk);
    torsionParameters.dxpj_dkj_.makeCrossProductMatrix(-vecji);
    torsionParameters.dxpk_dkj_.makeCrossProductMatrix(-veckl);
    torsionParameters.dxpk_dlk_.makeCrossProductMatrix(vecjk);
    torsionParameters.dcos_dxpj_ = (xpk - xpj * dp) / magxpj;
    torsionParameters.dcos_dxpk_ = (xpj - xpk * dp) / magxpk;

    return torsionParameters;
}

// Calculate SpeciesTorsion forces
void ForceKernel::forces(const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                         ForceVector &f) const
{
    // Calculate vectors, ensuring we account for minimum image
    Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;

    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);

    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, i.arrayIndex(), torsionParameters, f);
    addTorsionForceJ(du_dphi, j.arrayIndex(), torsionParameters, f);
    addTorsionForceK(du_dphi, k.arrayIndex(), torsionParameters, f);
    addTorsionForceL(du_dphi, l.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesTorsion forces
void ForceKernel::forces(const SpeciesTorsion &torsion, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                         const Vec3<double> &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, torsion.i()->index(), torsionParameters, f);
    addTorsionForceJ(du_dphi, torsion.j()->index(), torsionParameters, f);
    addTorsionForceK(du_dphi, torsion.k()->index(), torsionParameters, f);
    addTorsionForceL(du_dphi, torsion.l()->index(), torsionParameters, f);
}

// Calculate SpeciesImproper forces
void ForceKernel::forces(const SpeciesImproper &improper, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                         ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = improper.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, i.arrayIndex(), torsionParameters, f);
    addTorsionForceJ(du_dphi, j.arrayIndex(), torsionParameters, f);
    addTorsionForceK(du_dphi, k.arrayIndex(), torsionParameters, f);
    addTorsionForceL(du_dphi, l.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesImproper forces
void ForceKernel::forces(const SpeciesImproper &imp, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                         const Vec3<double> &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, imp.i()->index(), torsionParameters, f);
    addTorsionForceJ(du_dphi, imp.j()->index(), torsionParameters, f);
    addTorsionForceK(du_dphi, imp.k()->index(), torsionParameters, f);
    addTorsionForceL(du_dphi, imp.l()->index(), torsionParameters, f);
}

// Calculate intramolecular forces within the specified molecule
void ForceKernel::intramolecularForces(Molecule *mol, ForceVector &f, bool includePairPotentialTerms) const
{
    // Loop over bonds
    for (const auto &bond : mol->species()->bonds())
        forces(bond, *mol->atom(bond.indexI()), *mol->atom(bond.indexJ()), f);

    // Loop over angles
    for (const auto &angle : mol->species()->angles())
        forces(angle, *mol->atom(angle.indexI()), *mol->atom(angle.indexJ()), *mol->atom(angle.indexK()), f);

    // Loop over torsions
    for (const auto &torsion : mol->species()->torsions())
        forces(torsion, *mol->atom(torsion.indexI()), *mol->atom(torsion.indexJ()), *mol->atom(torsion.indexK()),
               *mol->atom(torsion.indexL()), f);

    // Loop over impropers
    for (const auto &imp : mol->species()->impropers())
        forces(imp, *mol->atom(imp.indexI()), *mol->atom(imp.indexJ()), *mol->atom(imp.indexK()), *mol->atom(imp.indexL()), f);

    // Pair potential interactions between atoms
    if (includePairPotentialTerms)
        dissolve::for_each_pair(ParallelPolicies::seq, mol->atoms().begin(), mol->atoms().end(),
                                [&](int indexI, const auto &i, int indexJ, const auto &j)
                                {
                                    if (indexI == indexJ)
                                        return;
                                    auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                        forcesWithMim(*i, *j, f);
                                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                        forcesWithMim(*i, *j, f, elec14, vdw14);
                                });
}

// Calculate total intramolecular forces in the world
void ForceKernel::totalIntramolecularForces(ForceVector &f, bool includePairPotentialTerms,
                                            ProcessPool::DivisionStrategy strategy) const
{
    assert(molecules_);
    auto &molecules = molecules_->get();

    auto combinable = createCombinableForces(f);

    auto moleculeForceOperator = [&](const auto &mol)
    {
        auto &fLocal = combinable.local();

        intramolecularForces(mol.get(), fLocal, includePairPotentialTerms);
    };

    // Set start/stride for parallel loop
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(molecules.begin(), molecules.end(), stride, start);
    dissolve::for_each(ParallelPolicies::par, begin, end, moleculeForceOperator);

    combinable.finalize();
}
