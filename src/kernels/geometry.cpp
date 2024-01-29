// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/geometry.h"
#include "classes/atomVector.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "templates/algorithms.h"

GeometryKernel::GeometryKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                               std::optional<double> energyCutoff)
    : KernelBase(cfg, procPool, potentialMap, energyCutoff)
{
}

GeometryKernel::GeometryKernel(const Box *box, const ProcessPool &procPool, const PotentialMap &potentialMap,
                               std::optional<double> energyCutoff)
    : KernelBase(box, procPool, potentialMap, energyCutoff)
{
}

/*
 * Bond Terms
 */

// Return SpeciesBond energy at Atoms specified
double GeometryKernel::bondEnergy(const SpeciesBond &b, const AtomRef i, const AtomRef j) const
{
    return b.energy(box_->minimumDistance(i.r(), j.r()));
}

// Calculate SpeciesBond forces
void GeometryKernel::bondForces(const SpeciesBond &bond, const AtomRef i, int indexI, const AtomRef j, int indexJ,
                                ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    f[indexI] -= vecji;
    f[indexJ] += vecji;
}

// Calculate SpeciesBond forces
void GeometryKernel::bondForces(const SpeciesBond &bond, const Vec3<double> &ri, const Vec3<double> &rj, ForceVector &f) const
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

/*
 * Angle Terms
 */

// Return SpeciesAngle energy at Atoms specified
double GeometryKernel::angleEnergy(const SpeciesAngle &a, const AtomRef i, const AtomRef j, const AtomRef k) const
{
    return a.energy(Box::angleInDegrees(box_->minimumVectorN(j.r(), i.r()), box_->minimumVectorN(j.r(), k.r())));
}

// Calculate angle force parameters from supplied vectors, storing results in supplied variables
GeometryKernel::AngleParameters GeometryKernel::calculateAngleForceParameters(Vec3<double> vecji, Vec3<double> vecjk)
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
void GeometryKernel::angleForces(const SpeciesAngle &angle, const AtomRef i, int indexI, const AtomRef j, int indexJ, const AtomRef k,
                                 int indexK, ForceVector &f) const
{
    auto vecji = box_->minimumVector(j.r(), i.r());
    auto vecjk = box_->minimumVector(j.r(), k.r());

    auto angleParameters = calculateAngleForceParameters(vecji, vecjk);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    f[indexI] += angleParameters.dfi_dtheta_;
    f[indexJ] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    f[indexK] += angleParameters.dfk_dtheta_;
}

// Calculate SpeciesAngle forces
void GeometryKernel::angleForces(const SpeciesAngle &angle, const Vec3<double> &ri, const Vec3<double> &rj,
                                 const Vec3<double> &rk, ForceVector &f) const
{
    auto angleParameters = calculateAngleForceParameters(ri - rj, rk - rj);
    const auto force = angle.force(angleParameters.theta_);
    angleParameters.dfi_dtheta_ *= force;
    angleParameters.dfk_dtheta_ *= force;

    // Store forces
    f[angle.i()->index()] += angleParameters.dfi_dtheta_;
    f[angle.j()->index()] -= angleParameters.dfi_dtheta_ + angleParameters.dfk_dtheta_;
    f[angle.k()->index()] += angleParameters.dfk_dtheta_;
}

/*
 * Torsion Terms
 */

// Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceI(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    auto &dcos_dxpj = torsionParameters.dcos_dxpj_;

    f[index].add(du_dphi * torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dij_.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpj_.dp(torsionParameters.dxpj_dij_.columnAsVec3(1)),
                 du_dphi * dcos_dxpj.dp(torsionParameters.dxpj_dij_.columnAsVec3(2)));
}

// Add torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceJ(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
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
void GeometryKernel::addTorsionForceK(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
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
void GeometryKernel::addTorsionForceL(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    f[index].add(du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(1)),
                 du_dphi * torsionParameters.dcos_dxpk_.dp(torsionParameters.dxpk_dlk_.columnAsVec3(2)));
}

// Return SpeciesTorsion energy at Atoms specified
double GeometryKernel::torsionEnergy(const SpeciesTorsion &t, const AtomRef i, const AtomRef j, const AtomRef k, const AtomRef l) const
{
    return t.energy(Box::torsionInDegrees(box_->minimumVector(j.r(), i.r()), box_->minimumVector(j.r(), k.r()),
                                          box_->minimumVector(k.r(), l.r())));
}

// Calculate torsion force parameters from supplied vectors, storing results in supplied variables
GeometryKernel::TorsionParameters
GeometryKernel::calculateTorsionForceParameters(const Vec3<double> &vecji, const Vec3<double> &vecjk, const Vec3<double> &veckl)
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
void GeometryKernel::torsionForces(const SpeciesTorsion &torsion, const AtomRef i, int indexI, const AtomRef j, int indexJ,
                                   const AtomRef k, int indexK, const AtomRef l, int indexL, ForceVector &f) const
{
    // Calculate vectors, ensuring we account for minimum image
    Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;

    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);

    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, indexI, torsionParameters, f);
    addTorsionForceJ(du_dphi, indexJ, torsionParameters, f);
    addTorsionForceK(du_dphi, indexK, torsionParameters, f);
    addTorsionForceL(du_dphi, indexL, torsionParameters, f);
}

// Calculate SpeciesTorsion forces
void GeometryKernel::torsionForces(const SpeciesTorsion &torsion, const Vec3<double> &ri, const Vec3<double> &rj,
                                   const Vec3<double> &rk, const Vec3<double> &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, torsion.i()->index(), torsionParameters, f);
    addTorsionForceJ(du_dphi, torsion.j()->index(), torsionParameters, f);
    addTorsionForceK(du_dphi, torsion.k()->index(), torsionParameters, f);
    addTorsionForceL(du_dphi, torsion.l()->index(), torsionParameters, f);
}

/*
 * Improper Terms
 */

// Return SpeciesImproper energy at Atoms specified
double GeometryKernel::improperEnergy(const SpeciesImproper &imp, const AtomRef i, const AtomRef j, const AtomRef k,
                                      const AtomRef l) const
{
    return imp.energy(Box::torsionInDegrees(box_->minimumVector(j.r(), i.r()), box_->minimumVector(j.r(), k.r()),
                                            box_->minimumVector(k.r(), l.r())));
}

// Calculate SpeciesImproper forces
void GeometryKernel::improperForces(const SpeciesImproper &improper, const AtomRef i, int indexI, const AtomRef j, int indexJ,
                                    const AtomRef k, int indexK, const AtomRef l, int indexL, ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = improper.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, indexI, torsionParameters, f);
    addTorsionForceJ(du_dphi, indexJ, torsionParameters, f);
    addTorsionForceK(du_dphi, indexK, torsionParameters, f);
    addTorsionForceL(du_dphi, indexL, torsionParameters, f);
}

// Calculate SpeciesImproper forces
void GeometryKernel::improperForces(const SpeciesImproper &imp, const Vec3<double> &ri, const Vec3<double> &rj,
                                    const Vec3<double> &rk, const Vec3<double> &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(torsionParameters.phi_ * DEGRAD);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, imp.i()->index(), torsionParameters, f);
    addTorsionForceJ(du_dphi, imp.j()->index(), torsionParameters, f);
    addTorsionForceK(du_dphi, imp.k()->index(), torsionParameters, f);
    addTorsionForceL(du_dphi, imp.l()->index(), torsionParameters, f);
}

/*
 * Total Energy
 */

// Return total energy for the geometry terms involving the specified atom
double GeometryKernel::totalGeometryEnergy(const AtomRef i) const
{
    // Get the SpeciesAtom and Molecule
    const auto *spAtom = i.speciesAtom();
    assert(spAtom);
    const auto &mol = *i.molecule();

    // If no terms are present, return zero
    if ((spAtom->nBonds() == 0) && (spAtom->nAngles() == 0) && (spAtom->nTorsions() == 0))
        return 0.0;

    auto intraEnergy = 0.0;

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(spAtom->bonds().begin(), spAtom->bonds().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesBond &bond)
                                   { return acc + bondEnergy(bond, *mol.atom(bond.indexI()), *mol.atom(bond.indexJ())); });

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(
        spAtom->angles().begin(), spAtom->angles().end(), 0.0,
        [this, &mol](const auto acc, const SpeciesAngle &angle)
        { return acc + angleEnergy(angle, *mol.atom(angle.indexI()), *mol.atom(angle.indexJ()), *mol.atom(angle.indexK())); });

    // Add energy from SpeciesTorsion terms
    intraEnergy +=
        std::accumulate(spAtom->torsions().begin(), spAtom->torsions().end(), 0.0,
                        [this, &mol](const auto acc, const SpeciesTorsion &torsion)
                        {
                            return acc + torsionEnergy(torsion, *mol.atom(torsion.indexI()), *mol.atom(torsion.indexJ()),
                                                       *mol.atom(torsion.indexK()), *mol.atom(torsion.indexL()));
                        });

    // Add energy from SpeciesImproper terms
    intraEnergy +=
        std::accumulate(spAtom->impropers().begin(), spAtom->impropers().end(), 0.0,
                        [this, &mol](const auto acc, const SpeciesImproper &improper)
                        {
                            return acc + improperEnergy(improper, *mol.atom(improper.indexI()), *mol.atom(improper.indexJ()),
                                                        *mol.atom(improper.indexK()), *mol.atom(improper.indexL()));
                        });

    return intraEnergy;
}

// Return total energy for the geometry terms in the specified molecule
double GeometryKernel::totalGeometryEnergy(const Molecule &mol) const
{
    auto intraEnergy = 0.0;

    // Loop over Bonds
    intraEnergy = dissolve::transform_reduce(
        ParallelPolicies::par, mol.species()->bonds().begin(), mol.species()->bonds().end(), intraEnergy, std::plus<double>(),
        [&mol, this](const auto &bond) { return bondEnergy(bond, *mol.atom(bond.indexI()), *mol.atom(bond.indexJ())); });

    // Loop over Angles
    intraEnergy = dissolve::transform_reduce(
        ParallelPolicies::seq, mol.species()->angles().begin(), mol.species()->angles().end(), intraEnergy, std::plus<double>(),
        [&mol, this](const auto &angle) -> double
        { return angleEnergy(angle, *mol.atom(angle.indexI()), *mol.atom(angle.indexJ()), *mol.atom(angle.indexK())); });

    // Loop over Torsions
    intraEnergy =
        dissolve::transform_reduce(ParallelPolicies::par, mol.species()->torsions().begin(), mol.species()->torsions().end(),
                                   intraEnergy, std::plus<double>(),
                                   [&mol, this](const auto &torsion) -> double
                                   {
                                       return torsionEnergy(torsion, *mol.atom(torsion.indexI()), *mol.atom(torsion.indexJ()),
                                                            *mol.atom(torsion.indexK()), *mol.atom(torsion.indexL()));
                                   });

    // Loop over Impropers
    intraEnergy = dissolve::transform_reduce(ParallelPolicies::par, mol.species()->impropers().begin(),
                                             mol.species()->impropers().end(), intraEnergy, std::plus<double>(),
                                             [&mol, this](const auto &improper) -> double
                                             {
                                                 return improperEnergy(
                                                     improper, *mol.atom(improper.indexI()), *mol.atom(improper.indexJ()),
                                                     *mol.atom(improper.indexK()), *mol.atom(improper.indexL()));
                                             });

    return intraEnergy;
}

// Calculate total forces within the specified molecule arising from geometry terms
void GeometryKernel::totalGeometryForces(const Molecule &mol, ForceVector &f) const
{
    const auto offset = mol.atoms()[0].globalAtomIndex();

    // Loop over bonds
    for (const auto &bond : mol.species()->bonds())
        bondForces(bond, *mol.atom(bond.indexI()), offset + bond.indexI(), *mol.atom(bond.indexJ()), offset + bond.indexJ(), f);

    // Loop over angles
    for (const auto &angle : mol.species()->angles())
        angleForces(angle, *mol.atom(angle.indexI()), offset + angle.indexI(), *mol.atom(angle.indexJ()),
                    offset + angle.indexJ(), *mol.atom(angle.indexK()), offset + angle.indexK(), f);

    // Loop over torsions
    for (const auto &torsion : mol.species()->torsions())
        torsionForces(torsion, *mol.atom(torsion.indexI()), offset + torsion.indexI(), *mol.atom(torsion.indexJ()),
                      offset + torsion.indexJ(), *mol.atom(torsion.indexK()), offset + torsion.indexK(),
                      *mol.atom(torsion.indexL()), offset + torsion.indexL(), f);

    // Loop over impropers
    for (const auto &imp : mol.species()->impropers())
        improperForces(imp, *mol.atom(imp.indexI()), offset + imp.indexI(), *mol.atom(imp.indexJ()), offset + imp.indexJ(),
                       *mol.atom(imp.indexK()), offset + imp.indexK(), *mol.atom(imp.indexL()), offset + imp.indexL(), f);
}
