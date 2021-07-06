// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/forcekernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>

ForceKernel::ForceKernel(ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap, double cutoffDistance)
    : box_(box), potentialMap_(potentialMap), processPool_(procPool)
{
    cutoffDistanceSquared_ =
        (cutoffDistance < 0.0 ? potentialMap_.range() * potentialMap_.range() : cutoffDistance * cutoffDistance);
}

/*
 * Internal Force Calculation
 */

// Calculate PairPotential forces between Atoms provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f, double scale) const
{
    auto force = j.r() - i.r();
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r) * scale;
    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

// Calculate PairPotential forces between Atoms provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Atom &i, const Atom &j, ForceVector &f, double scale) const
{
    auto force = box_->minimumVector(i.r(), j.r());
    auto distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r) * scale;

    f[i.arrayIndex()] += force;
    f[j.arrayIndex()] -= force;
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms
void ForceKernel::forces(const Atom &i, const Atom &j, bool applyMim, bool excludeIgeJ, ForceVector &f) const
{
    // If Atoms are the same, we refuse to calculate
    if (&i == &j)
        return;

    // Check indices of atoms if required
    if (excludeIgeJ && (i.arrayIndex() >= j.arrayIndex()))
        return;

    if (applyMim)
        forcesWithMim(i, j, f);
    else
        forcesWithoutMim(i, j, f);
}

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
            for (auto &jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii->arrayIndex() >= jj->arrayIndex()))
                    continue;

                // Check for atoms in the same Molecule
                if (molI != jj->molecule())
                    forcesWithMim(*ii, *jj, f);
                else
                {
                    double scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(*ii, *jj, f, scale);
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
                    double scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(*ii, *jj, f, scale);
                }
            }
        }
    }
}

// Calculate forces between Cell and its neighbours
void ForceKernel::forces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy, ForceVector &f) const
{
    // Straight loop over Cells *not* requiring mim
    for (auto *otherCell : cell->cellNeighbours())
        forces(cell, otherCell, false, excludeIgeJ, strategy, f);

    // Straight loop over Cells requiring mim
    for (auto *otherCell : cell->mimCellNeighbours())
        forces(cell, otherCell, true, excludeIgeJ, strategy, f);
}

// Calculate forces between Atom and Cell
void ForceKernel::forces(const Atom &i, const Cell *cell, int flags, ProcessPool::DivisionStrategy strategy,
                         ForceVector &f) const
{
    // Grab some information on the supplied atom
    auto moleculeI = i.molecule();

    // Grab the array of Atoms in the supplied Cell
    auto &otherAtoms = cell->atoms();

    // Get start/stride for specified loop context
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    // Loop over cell atoms
    if (flags & KernelFlags::ApplyMinimumImageFlag)
    {
        // Loop over atom neighbours
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto &jj : otherAtoms)
            {
                // Check for same atom
                if (&i == jj)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, *jj, f);
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (const auto &jj : otherAtoms)
            {
                // Check for i >= jj
                if (i.arrayIndex() >= jj->arrayIndex())
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, *jj, f, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
        {
            auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, *jj, f);
                else
                {
                    // Check for i >= jj
                    if (i.arrayIndex() >= jj->arrayIndex())
                        continue;

                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, *jj, f, scale);
                }
            }
        }
        else
        {
            auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, *jj, f, scale);
                }
            }
        }
    }
    else
    {
        // Loop over atom neighbours
        auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for same atom
                if (&i == jj)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, *jj, f, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for i >= jj
                if (i.arrayIndex() >= jj->arrayIndex())
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, *jj, f, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, *jj, f);
                else
                {
                    // Pointer comparison for i >= jj
                    if (&i >= jj)
                        continue;

                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, *jj, f, scale);
                }
            }
        else
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                auto &jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, *jj, f);
                else
                {
                    double scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, *jj, f, scale);
                }
            }
    }
}

// Calculate forces between atom and world
void ForceKernel::forces(const Atom &i, ProcessPool::DivisionStrategy strategy, ForceVector &f) const
{
    Cell *cellI = i.cell();

    // This Atom with other Atoms in the same Cell
    forces(i, cellI, KernelFlags::ExcludeSelfFlag, strategy, f);

    // This Atom with other Atoms in neighbour Cells
    for (auto *neighbour : cellI->cellNeighbours())
        forces(i, neighbour, KernelFlags::NoFlags, strategy, f);

    // This Atom with other Atoms in neighbour Cells which require minimum image
    for (auto *neighbour : cellI->mimCellNeighbours())
        forces(i, neighbour, KernelFlags::ApplyMinimumImageFlag, strategy, f);
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
    // Determine whether we need to apply minimum image to the vector calculation
    Vec3<double> vecji;
    if (i.cell()->mimRequired(j.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    f[i.arrayIndex()] -= vecji;
    f[j.arrayIndex()] += vecji;
}

// Calculate SpeciesBond forces for specified Atom only
void ForceKernel::forces(const Atom &onlyThis, const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const
{
    // Determine whether we need to apply minimum image to the vector calculation
    Vec3<double> vecji;
    if (i.cell()->mimRequired(j.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    if (&onlyThis == &i)
        f[onlyThis.arrayIndex()] -= vecji;
    else
        f[onlyThis.arrayIndex()] += vecji;
}

// Calculate SpeciesBond forces
void ForceKernel::forces(const SpeciesBond &bond, ForceVector &f) const
{
    auto vecji = bond.j()->r() - bond.i()->r();

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
    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(j.r(), i.r());
    else
        vecji = i.r() - j.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(j.r(), k.r());
    else
        vecjk = k.r() - j.r();

    auto angleParameters = calculateAngleParameters(vecji, vecjk);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    f[i.arrayIndex()] += angleParameters.dfi_dtheta_;
    f[j.arrayIndex()] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    f[k.arrayIndex()] += angleParameters.dfk_dtheta_;
}

// Calculate SpeciesAngle forces for specified Atom only
void ForceKernel::forces(const Atom &onlyThis, const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k,
                         ForceVector &f) const
{
    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(j.r(), i.r());
    else
        vecji = i.r() - j.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(j.r(), k.r());
    else
        vecjk = k.r() - j.r();

    auto angleParameters = calculateAngleParameters(vecji, vecjk);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    if (&onlyThis == &i)
        f[onlyThis.arrayIndex()] += angleParameters.dfi_dtheta_;
    else if (&onlyThis == &j)
        f[onlyThis.arrayIndex()] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    else
        f[onlyThis.arrayIndex()] += angleParameters.dfk_dtheta_;
}

// Calculate SpeciesAngle forces
void ForceKernel::forces(const SpeciesAngle &angle, ForceVector &f) const
{
    auto angleParameters = calculateAngleParameters(angle.i()->r() - angle.j()->r(), angle.k()->r() - angle.j()->r());
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
    Vec3<double> vecji, vecjk, veckl, dcos_dxpj, dcos_dxpk;
    Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(k.r(), j.r());
    else
        vecjk = j.r() - k.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(l.r(), k.r());
    else
        veckl = k.r() - l.r();

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);

    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, i.arrayIndex(), torsionParameters, f);
    addTorsionForceJ(du_dphi, j.arrayIndex(), torsionParameters, f);
    addTorsionForceK(du_dphi, k.arrayIndex(), torsionParameters, f);
    addTorsionForceL(du_dphi, l.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesTorsion forces for specified Atom only
void ForceKernel::forces(const Atom &onlyThis, const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k,
                         const Atom &l, ForceVector &f) const
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(k.r(), j.r());
    else
        vecjk = j.r() - k.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(l.r(), k.r());
    else
        veckl = k.r() - l.r();

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces for specified atom
    if (&onlyThis == &i)
        addTorsionForceI(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else if (&onlyThis == &j)
        addTorsionForceJ(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else if (&onlyThis == &k)
        addTorsionForceK(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else
        addTorsionForceL(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesTorsion forces
void ForceKernel::forces(const SpeciesTorsion &torsion, ForceVector &f) const
{
    // Calculate vectors, ensuring we account for minimum image
    const Vec3<double> vecji = torsion.j()->r() - torsion.i()->r(), vecjk = torsion.j()->r() - torsion.k()->r(),
                       veckl = torsion.k()->r() - torsion.l()->r();

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
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(k.r(), j.r());
    else
        vecjk = j.r() - k.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(l.r(), k.r());
    else
        veckl = k.r() - l.r();

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = improper.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, i.arrayIndex(), torsionParameters, f);
    addTorsionForceJ(du_dphi, j.arrayIndex(), torsionParameters, f);
    addTorsionForceK(du_dphi, k.arrayIndex(), torsionParameters, f);
    addTorsionForceL(du_dphi, l.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesImproper forces for specified Atom only
void ForceKernel::forces(const Atom &onlyThis, const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k,
                         const Atom &l, ForceVector &f) const
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(i.r(), j.r());
    else
        vecji = j.r() - i.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(k.r(), j.r());
    else
        vecjk = j.r() - k.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(l.r(), k.r());
    else
        veckl = k.r() - l.r();

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces for specified atom
    if (&onlyThis == &i)
        addTorsionForceI(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else if (&onlyThis == &j)
        addTorsionForceJ(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else if (&onlyThis == &k)
        addTorsionForceK(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
    else
        addTorsionForceL(du_dphi, onlyThis.arrayIndex(), torsionParameters, f);
}

// Calculate SpeciesImproper forces
void ForceKernel::forces(const SpeciesImproper &imp, ForceVector &f) const
{
    // Calculate vectors
    const auto vecji = imp.j()->r() - imp.i()->r(), vecjk = imp.j()->r() - imp.k()->r(), veckl = imp.k()->r() - imp.l()->r();

    auto torsionParameters = calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, imp.i()->index(), torsionParameters, f);
    addTorsionForceJ(du_dphi, imp.j()->index(), torsionParameters, f);
    addTorsionForceK(du_dphi, imp.k()->index(), torsionParameters, f);
    addTorsionForceL(du_dphi, imp.l()->index(), torsionParameters, f);
}
