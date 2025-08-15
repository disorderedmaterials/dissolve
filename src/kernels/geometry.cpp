// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "kernels/geometry.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "math/mathFunc.h"
#include "templates/algorithms.h"

GeometryKernel::GeometryKernel(const Configuration *cfg, const PotentialMap &potentialMap, std::optional<double> energyCutoff)
    : KernelBase(cfg, potentialMap, energyCutoff)
{
}

GeometryKernel::GeometryKernel(const Box *box, const PotentialMap &potentialMap, std::optional<double> energyCutoff)
    : KernelBase(box, potentialMap, energyCutoff)
{
}

/*
 * Bond Terms
 */

// Return SpeciesBond energy at Atoms specified
double GeometryKernel::bondEnergy(const SpeciesBond &b, const Atom &i, const Atom &j) const
{
    return b.energy(box_->minimumDistance(i.r(), j.r()));
}

// Calculate SpeciesBond forces
void GeometryKernel::bondForces(const SpeciesBond &bond, const Atom &i, int indexI, const Atom &j, int indexJ,
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
void GeometryKernel::bondForces(const SpeciesBond &bond, const Vector3 &ri, const Vector3 &rj, ForceVector &f) const
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
double GeometryKernel::angleEnergy(const SpeciesAngle &a, const Atom &i, const Atom &j, const Atom &k) const
{
    return a.energy(box_->angleInRadians(i.r(), j.r(), k.r()));
}

// Calculate angle force parameters from supplied vectors, storing results in supplied variables
GeometryKernel::AngleParameters GeometryKernel::calculateAngleForceParameters(Vector3 vecji, Vector3 vecjk)
{
    // Calculate angle
    AngleParameters angleParameters;
    const auto magji = vecji.magAndNormalise();
    const auto magjk = vecjk.magAndNormalise();
    auto dp = vecji.dp(vecjk);
    angleParameters.theta = vecji.angleInRadians(vecjk);

    // Determine force vectors for atoms
    angleParameters.dfi_dtheta = (vecjk - vecji * dp) / magji;
    angleParameters.dfk_dtheta = (vecji - vecjk * dp) / magjk;

    return angleParameters;
}

// Calculate SpeciesAngle forces
void GeometryKernel::angleForces(const SpeciesAngle &angle, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                                 int indexK, ForceVector &f) const
{
    auto vecji = box_->minimumVector(j.r(), i.r());
    auto vecjk = box_->minimumVector(j.r(), k.r());

    auto angleParameters = calculateAngleForceParameters(vecji, vecjk);
    const auto force = angle.force(angleParameters.theta);
    angleParameters.dfi_dtheta *= force;
    angleParameters.dfk_dtheta *= force;

    // Store forces
    f[indexI] += angleParameters.dfi_dtheta;
    f[indexJ] -= angleParameters.dfi_dtheta + angleParameters.dfk_dtheta;
    f[indexK] += angleParameters.dfk_dtheta;
}

// Calculate SpeciesAngle forces
void GeometryKernel::angleForces(const SpeciesAngle &angle, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                                 ForceVector &f) const
{
    auto angleParameters = calculateAngleForceParameters(ri - rj, rk - rj);
    const auto force = angle.force(angleParameters.theta);
    angleParameters.dfi_dtheta *= force;
    angleParameters.dfk_dtheta *= force;

    // Store forces
    f[angle.i()->index()] += angleParameters.dfi_dtheta;
    f[angle.j()->index()] -= angleParameters.dfi_dtheta + angleParameters.dfk_dtheta;
    f[angle.k()->index()] += angleParameters.dfk_dtheta;
}

/*
 * Torsion Terms
 */

// Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceI(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    auto &dcos_dxpj = torsionParameters.dcos_dxpj;

    f[index].add(du_dphi * torsionParameters.dcos_dxpj.dp(torsionParameters.dxpj_dij.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpj.dp(torsionParameters.dxpj_dij.columnAsVec3(1)),
                 du_dphi * dcos_dxpj.dp(torsionParameters.dxpj_dij.columnAsVec3(2)));
}

// Add torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceJ(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    f[index].add(du_dphi * (torsionParameters.dcos_dxpj.dp(-torsionParameters.dxpj_dij.columnAsVec3(0) -
                                                           torsionParameters.dxpj_dkj.columnAsVec3(0)) -
                            torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(0))),
                 du_dphi * (torsionParameters.dcos_dxpj.dp(-torsionParameters.dxpj_dij.columnAsVec3(1) -
                                                           torsionParameters.dxpj_dkj.columnAsVec3(1)) -
                            torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(1))),
                 du_dphi * (torsionParameters.dcos_dxpj.dp(-torsionParameters.dxpj_dij.columnAsVec3(2) -
                                                           torsionParameters.dxpj_dkj.columnAsVec3(2)) -
                            torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(2))));
}

// Add torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceK(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    f[index].add(du_dphi * (torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(0) -
                                                           torsionParameters.dxpk_dlk.columnAsVec3(0)) +
                            torsionParameters.dcos_dxpj.dp(torsionParameters.dxpj_dkj.columnAsVec3(0))),
                 du_dphi * (torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(1) -
                                                           torsionParameters.dxpk_dlk.columnAsVec3(1)) +
                            torsionParameters.dcos_dxpj.dp(torsionParameters.dxpj_dkj.columnAsVec3(1))),
                 du_dphi * (torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dkj.columnAsVec3(2) -
                                                           torsionParameters.dxpk_dlk.columnAsVec3(2)) +
                            torsionParameters.dcos_dxpj.dp(torsionParameters.dxpj_dkj.columnAsVec3(2))));
}

// Add torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index and input vector
void GeometryKernel::addTorsionForceL(double du_dphi, int index, GeometryKernel::TorsionParameters &torsionParameters,
                                      ForceVector &f) const
{
    f[index].add(du_dphi * torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dlk.columnAsVec3(0)),
                 du_dphi * torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dlk.columnAsVec3(1)),
                 du_dphi * torsionParameters.dcos_dxpk.dp(torsionParameters.dxpk_dlk.columnAsVec3(2)));
}

// Return SpeciesTorsion energy at Atoms specified
double GeometryKernel::torsionEnergy(const SpeciesTorsion &t, const Atom &i, const Atom &j, const Atom &k, const Atom &l) const
{
    return t.energy(box_->torsionInRadians(i.r(), j.r(), k.r(), l.r()));
}

// Calculate torsion force parameters from supplied vectors, storing results in supplied variables
GeometryKernel::TorsionParameters GeometryKernel::calculateTorsionForceParameters(const Vector3 &vecji, const Vector3 &vecjk,
                                                                                  const Vector3 &veckl)
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
    torsionParameters.phi = atan2(vecjk.dp(xpj * xpk) / vecjk.magnitude(), dp);
    torsionParameters.dxpj_dij.makeCrossProductMatrix(vecjk);
    torsionParameters.dxpj_dkj.makeCrossProductMatrix(-vecji);
    torsionParameters.dxpk_dkj.makeCrossProductMatrix(-veckl);
    torsionParameters.dxpk_dlk.makeCrossProductMatrix(vecjk);
    torsionParameters.dcos_dxpj = (xpk - xpj * dp) / magxpj;
    torsionParameters.dcos_dxpk = (xpj - xpk * dp) / magxpk;

    return torsionParameters;
}

// Calculate SpeciesTorsion forces
void GeometryKernel::torsionForces(const SpeciesTorsion &torsion, const Atom &i, int indexI, const Atom &j, int indexJ,
                                   const Atom &k, int indexK, const Atom &l, int indexL, ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);

    const auto du_dphi = torsion.force(torsionParameters.phi);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, indexI, torsionParameters, f);
    addTorsionForceJ(du_dphi, indexJ, torsionParameters, f);
    addTorsionForceK(du_dphi, indexK, torsionParameters, f);
    addTorsionForceL(du_dphi, indexL, torsionParameters, f);
}

// Calculate SpeciesTorsion forces
void GeometryKernel::torsionForces(const SpeciesTorsion &torsion, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                                   const Vector3 &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(torsionParameters.phi);

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
double GeometryKernel::improperEnergy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k,
                                      const Atom &l) const
{
    return imp.energy(box_->torsionInRadians(i.r(), j.r(), k.r(), l.r()));
}

// Calculate SpeciesImproper forces
void GeometryKernel::improperForces(const SpeciesImproper &improper, const Atom &i, int indexI, const Atom &j, int indexJ,
                                    const Atom &k, int indexK, const Atom &l, int indexL, ForceVector &f) const
{
    auto vecji = box_->minimumVector(i.r(), j.r());
    auto vecjk = box_->minimumVector(k.r(), j.r());
    auto veckl = box_->minimumVector(l.r(), k.r());

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = improper.force(torsionParameters.phi);

    // Sum forces on atoms
    addTorsionForceI(du_dphi, indexI, torsionParameters, f);
    addTorsionForceJ(du_dphi, indexJ, torsionParameters, f);
    addTorsionForceK(du_dphi, indexK, torsionParameters, f);
    addTorsionForceL(du_dphi, indexL, torsionParameters, f);
}

// Calculate SpeciesImproper forces
void GeometryKernel::improperForces(const SpeciesImproper &imp, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                                    const Vector3 &rl, ForceVector &f) const
{
    auto vecji = box_->minimumVector(ri, rj);
    auto vecjk = box_->minimumVector(rk, rj);
    auto veckl = box_->minimumVector(rl, rk);

    auto torsionParameters = calculateTorsionForceParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(torsionParameters.phi);

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
double GeometryKernel::totalGeometryEnergy(const Atom &i) const
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
    const auto offset = mol.globalAtomOffset();

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
