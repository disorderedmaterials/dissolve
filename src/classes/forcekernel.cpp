// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/forcekernel.h"
#include "base/processpool.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>

ForceKernel::ForceKernel(ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap, Array<double> &fx,
                         Array<double> &fy, Array<double> &fz, double cutoffDistance)
    : box_(box), potentialMap_(potentialMap), fx_(fx), fy_(fy), fz_(fz), processPool_(procPool)
{
    cutoffDistanceSquared_ =
        (cutoffDistance < 0.0 ? potentialMap_.range() * potentialMap_.range() : cutoffDistance * cutoffDistance);
}

/*
 * Internal Force Calculation
 */

// Calculate PairPotential forces between Atoms provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double scale)
{
    Vec3<double> force = j->r() - i->r();
    double distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    double r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r) * scale;

    auto index = i->arrayIndex();
    fx_[index] += force.x;
    fy_[index] += force.y;
    fz_[index] += force.z;
    index = j->arrayIndex();
    fx_[index] -= force.x;
    fy_[index] -= force.y;
    fz_[index] -= force.z;
}

// Calculate PairPotential forces between Atoms provided (minimum image calculation)
void ForceKernel::forcesWithMim(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double scale)
{
    Vec3<double> force = box_->minimumVector(i, j);
    double distanceSq = force.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    double r = sqrt(distanceSq);
    force /= r;
    force *= potentialMap_.force(i, j, r) * scale;

    auto index = i->arrayIndex();
    fx_[index] += force.x;
    fy_[index] += force.y;
    fz_[index] += force.z;
    index = j->arrayIndex();
    fx_[index] -= force.x;
    fy_[index] -= force.y;
    fz_[index] -= force.z;
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms
void ForceKernel::forces(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, bool applyMim, bool excludeIgeJ)
{
    assert(i && j);

    // If Atoms are the same, we refuse to calculate
    if (i == j)
        return;

    // Check indices of atoms if required
    if (excludeIgeJ && (i->arrayIndex() >= j->arrayIndex()))
        return;

    if (applyMim)
        forcesWithMim(i, j);
    else
        forcesWithoutMim(i, j);
}

// Calculate forces between atoms in supplied cells
void ForceKernel::forces(Cell *centralCell, Cell *otherCell, bool applyMim, bool excludeIgeJ,
                         ProcessPool::DivisionStrategy strategy)
{
    assert(centralCell && otherCell);

    auto &centralAtoms = centralCell->atoms();
    auto &otherAtoms = otherCell->atoms();
    std::shared_ptr<Atom> ii;
    Vec3<double> rI;
    std::shared_ptr<Molecule> molI;
    double scale;

    // Get start/stride for specified loop context
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    // Loop over central cell atoms
    if (applyMim)
    {
        auto [begin, end] = chop_range(centralAtoms.begin(), centralAtoms.end(), nChunks, offset);
        for (auto indexI = begin; indexI < end; ++indexI)
        {
            ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii->arrayIndex() >= jj->arrayIndex()))
                    continue;

                // Check for atoms in the same Molecule
                if (molI != jj->molecule())
                    forcesWithMim(ii, jj);
                else
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(ii, jj, scale);
                }
            }
        }
    }
    else
    {
        auto [begin, end] = chop_range(centralCell->atoms().begin(), centralCell->atoms().end(), nChunks, offset);
        for (auto indexI = begin; indexI < end; ++indexI)
        {
            ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii->arrayIndex() >= jj->arrayIndex()))
                    continue;

                // Check for atoms in the same molecule
                if (molI != jj->molecule())
                    forcesWithoutMim(ii, jj);
                else
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(ii, jj, scale);
                }
            }
        }
    }
}

// Calculate forces between Cell and its neighbours
void ForceKernel::forces(Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy)
{
    // Straight loop over Cells *not* requiring mim
    for (auto *otherCell : cell->cellNeighbours())
        forces(cell, otherCell, false, excludeIgeJ, strategy);

    // Straight loop over Cells requiring mim
    for (auto *otherCell : cell->mimCellNeighbours())
        forces(cell, otherCell, true, excludeIgeJ, strategy);
}

// Calculate forces between Atom and Cell
void ForceKernel::forces(const std::shared_ptr<Atom> i, Cell *cell, int flags, ProcessPool::DivisionStrategy strategy)
{
    assert(i);

    std::shared_ptr<Atom> jj;
    double scale;

    // Grab some information on the supplied atom
    auto moleculeI = i->molecule();

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
            for (auto jj : otherAtoms)
            {
                // Check for same atom
                if (i == jj)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, jj, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto jj : otherAtoms)
            {
                // Check for i >= jj
                if (i->arrayIndex() >= jj->arrayIndex())
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, jj, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
        {
            auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, jj);
                else
                {
                    // Check for i >= jj
                    if (i->arrayIndex() >= jj->arrayIndex())
                        continue;

                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, jj, scale);
                }
            }
        }
        else
        {
            auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithMim(i, jj, scale);
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
                jj = *indexJ;

                // Check for same atom
                if (i == jj)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, jj, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for i >= jj
                if (i->arrayIndex() >= jj->arrayIndex())
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, jj, scale);
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, jj);
                else
                {
                    // Pointer comparison for i >= jj
                    if (i >= jj)
                        continue;

                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, jj, scale);
                }
            }
        else
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    forcesWithoutMim(i, jj);
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        forcesWithoutMim(i, jj, scale);
                }
            }
    }
}

// Calculate forces between atom and world
void ForceKernel::forces(const std::shared_ptr<Atom> i, ProcessPool::DivisionStrategy strategy)
{
    assert(i);

    Cell *cellI = i->cell();

    // This Atom with other Atoms in the same Cell
    forces(i, cellI, KernelFlags::ExcludeSelfFlag, strategy);

    // This Atom with other Atoms in neighbour Cells
    for (auto *neighbour : cellI->cellNeighbours())
        forces(i, neighbour, KernelFlags::NoFlags, strategy);

    // This Atom with other Atoms in neighbour Cells which require minimum image
    for (auto *neighbour : cellI->mimCellNeighbours())
        forces(i, neighbour, KernelFlags::ApplyMinimumImageFlag, strategy);
}

/*
 * Intramolecular Terms
 */

// Calculate angle force parameters from supplied vectors, storing result in local class variables
void ForceKernel::calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk)
{
    calculateAngleParameters(vecji, vecjk, theta_, dfi_dtheta_, dfk_dtheta_);
}

// Calculate SpeciesTorsion forces parameters from supplied vectors, storing result in local class variables
void ForceKernel::calculateTorsionParameters(const Vec3<double> vecji, const Vec3<double> vecjk, const Vec3<double> veckl)
{
    calculateTorsionParameters(vecji, vecjk, veckl, phi_, dxpj_dij_, dxpj_dkj_, dxpk_dkj_, dxpk_dlk_, dcos_dxpj_, dcos_dxpk_);
}

// Calculate SpeciesBond forces
void ForceKernel::forces(const SpeciesBond &bond, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j)
{
    // Determine whether we need to apply minimum image to the vector calculation
    Vec3<double> vecji;
    if (i->cell()->mimRequired(j->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    auto index = i->arrayIndex();
    fx_[index] -= vecji.x;
    fy_[index] -= vecji.y;
    fz_[index] -= vecji.z;
    index = j->arrayIndex();
    fx_[index] += vecji.x;
    fy_[index] += vecji.y;
    fz_[index] += vecji.z;
}

// Calculate SpeciesBond forces for specified Atom only
void ForceKernel::forces(const std::shared_ptr<Atom> onlyThis, const SpeciesBond &bond, const std::shared_ptr<Atom> i,
                         const std::shared_ptr<Atom> j)
{
    assert((i != onlyThis) && (j != onlyThis));

    // Determine whether we need to apply minimum image to the vector calculation
    Vec3<double> vecji;
    if (i->cell()->mimRequired(j->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();

    // Get distance and normalise vector ready for force calculation
    auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    auto index = onlyThis->arrayIndex();
    if (onlyThis == i)
    {
        fx_[index] -= vecji.x;
        fy_[index] -= vecji.y;
        fz_[index] -= vecji.z;
    }
    else
    {
        fx_[index] += vecji.x;
        fy_[index] += vecji.y;
        fz_[index] += vecji.z;
    }
}

// Calculate SpeciesBond forces
void ForceKernel::forces(const SpeciesBond &bond)
{
    auto vecji = bond.j()->r() - bond.i()->r();

    // Get distance and normalise vector ready for force calculation
    const auto distance = vecji.magAndNormalise();

    // Determine final forces
    vecji *= bond.force(distance);

    // Calculate forces
    auto index = bond.i()->index();
    fx_[index] -= vecji.x;
    fy_[index] -= vecji.y;
    fz_[index] -= vecji.z;
    index = bond.j()->index();
    fx_[index] += vecji.x;
    fy_[index] += vecji.y;
    fz_[index] += vecji.z;
}

// Calculate angle force parameters from supplied vectors, storing results in supplied variables
void ForceKernel::calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk, double &theta, Vec3<double> &dfi_dtheta,
                                           Vec3<double> &dfk_dtheta)
{
    // Calculate angle
    const auto magji = vecji.magAndNormalise();
    const auto magjk = vecjk.magAndNormalise();
    double dp;
    theta = Box::angleInDegrees(vecji, vecjk, dp);

    // Determine force vectors for atoms
    dfi_dtheta = (vecjk - vecji * dp) / magji;
    dfk_dtheta = (vecji - vecjk * dp) / magjk;
}

// Calculate SpeciesAngle forces
void ForceKernel::forces(const SpeciesAngle &angle, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                         const std::shared_ptr<Atom> k)
{
    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i->r() - j->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k->r() - j->r();

    calculateAngleParameters(vecji, vecjk);
    const auto force = angle.force(theta_);
    dfi_dtheta_ *= force;
    dfk_dtheta_ *= force;

    // Store forces
    auto index = i->arrayIndex();
    fx_[index] += dfi_dtheta_.x;
    fy_[index] += dfi_dtheta_.y;
    fz_[index] += dfi_dtheta_.z;
    index = j->arrayIndex();
    fx_[index] -= dfi_dtheta_.x + dfk_dtheta_.x;
    fy_[index] -= dfi_dtheta_.y + dfk_dtheta_.y;
    fz_[index] -= dfi_dtheta_.z + dfk_dtheta_.z;
    index = k->arrayIndex();
    fx_[index] += dfk_dtheta_.x;
    fy_[index] += dfk_dtheta_.y;
    fz_[index] += dfk_dtheta_.z;
}

// Calculate SpeciesAngle forces for specified Atom only
void ForceKernel::forces(const std::shared_ptr<Atom> onlyThis, const SpeciesAngle &angle, const std::shared_ptr<Atom> i,
                         const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k)
{
    assert((i != onlyThis) && (j != onlyThis) && (k != onlyThis));

    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i->r() - j->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k->r() - j->r();

    calculateAngleParameters(vecji, vecjk);
    const auto force = angle.force(theta_);
    dfi_dtheta_ *= force;
    dfk_dtheta_ *= force;

    // Store forces
    auto index = onlyThis->arrayIndex();
    if (onlyThis == i)
    {
        fx_[index] += dfi_dtheta_.x;
        fy_[index] += dfi_dtheta_.y;
        fz_[index] += dfi_dtheta_.z;
    }
    else if (onlyThis == j)
    {
        fx_[index] -= dfi_dtheta_.x + dfk_dtheta_.x;
        fy_[index] -= dfi_dtheta_.y + dfk_dtheta_.y;
        fz_[index] -= dfi_dtheta_.z + dfk_dtheta_.z;
    }
    else
    {
        fx_[index] += dfk_dtheta_.x;
        fy_[index] += dfk_dtheta_.y;
        fz_[index] += dfk_dtheta_.z;
    }
}

// Calculate SpeciesAngle forces
void ForceKernel::forces(const SpeciesAngle &angle)
{
    calculateAngleParameters(angle.i()->r() - angle.j()->r(), angle.k()->r() - angle.j()->r());
    const auto force = angle.force(theta_);
    dfi_dtheta_ *= force;
    dfk_dtheta_ *= force;

    // Store forces
    auto index = angle.i()->index();
    fx_[index] += dfi_dtheta_.x;
    fy_[index] += dfi_dtheta_.y;
    fz_[index] += dfi_dtheta_.z;
    index = angle.j()->index();
    fx_[index] -= dfi_dtheta_.x + dfk_dtheta_.x;
    fy_[index] -= dfi_dtheta_.y + dfk_dtheta_.y;
    fz_[index] -= dfi_dtheta_.z + dfk_dtheta_.z;
    index = angle.k()->index();
    fx_[index] += dfk_dtheta_.x;
    fy_[index] += dfk_dtheta_.y;
    fz_[index] += dfk_dtheta_.z;
}

// Calculate torsion force parameters from supplied vectors, storing results in supplied variables
void ForceKernel::calculateTorsionParameters(const Vec3<double> vecji, const Vec3<double> vecjk, const Vec3<double> veckl,
                                             double &phi, Matrix3 &dxpj_dij, Matrix3 &dxpj_dkj, Matrix3 &dxpk_dkj,
                                             Matrix3 &dxpk_dlk, Vec3<double> &dcos_dxpj, Vec3<double> &dcos_dxpk)
{
    // Calculate cross products and torsion angle formed (in radians)
    Vec3<double> xpj = vecji * vecjk;
    Vec3<double> xpk = veckl * vecjk;
    const auto magxpj = xpj.magAndNormalise();
    const auto magxpk = xpk.magAndNormalise();
    auto dp = xpj.dp(xpk);
    phi = atan2(vecjk.dp(xpj * xpk) / vecjk.magnitude(), dp);

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

    dxpj_dij.makeCrossProductMatrix(vecjk);
    dxpj_dkj.makeCrossProductMatrix(-vecji);
    dxpk_dkj.makeCrossProductMatrix(-veckl);
    dxpk_dlk.makeCrossProductMatrix(vecjk);

    // Construct derivatives of cos(phi) w.r.t. perpendicular axes
    dcos_dxpj = (xpk - xpj * dp) / magxpj;
    dcos_dxpk = (xpj - xpk * dp) / magxpk;
}

// Calculate SpeciesTorsion forces
void ForceKernel::forces(const SpeciesTorsion &torsion, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                         const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l)
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(k, j);
    else
        vecjk = j->r() - k->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(l, k);
    else
        veckl = k->r() - l->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(phi_ * DEGRAD);

    // Calculate forces on atom i
    auto index = i->arrayIndex();
    fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));

    index = j->arrayIndex();
    fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));

    index = k->arrayIndex();
    fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));

    index = l->arrayIndex();
    fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
}

// Calculate SpeciesTorsion forces for specified Atom only
void ForceKernel::forces(const std::shared_ptr<Atom> onlyThis, const SpeciesTorsion &torsion, const std::shared_ptr<Atom> i,
                         const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l)
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(k, j);
    else
        vecjk = j->r() - k->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(l, k);
    else
        veckl = k->r() - l->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(phi_ * DEGRAD);

    // Calculate forces for specified atom
    auto index = onlyThis->arrayIndex();
    if (onlyThis == i)
    {
        fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
        fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
        fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));
    }
    else if (onlyThis == j)
    {
        fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
        fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
        fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));
    }
    else if (onlyThis == k)
    {
        fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
        fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
        fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));
    }
    else
    {
        fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
        fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
        fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
    }
}

// Calculate SpeciesTorsion forces
void ForceKernel::forces(const SpeciesTorsion &torsion)
{
    // Calculate vectors, ensuring we account for minimum image
    const Vec3<double> vecji = torsion.j()->r() - torsion.i()->r(), vecjk = torsion.j()->r() - torsion.k()->r(),
                       veckl = torsion.k()->r() - torsion.l()->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = torsion.force(phi_ * DEGRAD);

    // Calculate forces on atom i
    auto index = torsion.i()->index();
    fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));

    index = torsion.j()->index();
    fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));

    index = torsion.k()->index();
    fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));

    index = torsion.l()->index();
    fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
}

// Calculate SpeciesImproper forces
void ForceKernel::forces(const SpeciesImproper &improper, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                         const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l)
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(k, j);
    else
        vecjk = j->r() - k->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(l, k);
    else
        veckl = k->r() - l->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = improper.force(phi_ * DEGRAD);

    // Calculate forces on atom i
    auto index = i->arrayIndex();
    fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));

    index = j->arrayIndex();
    fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));

    index = k->arrayIndex();
    fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));

    index = l->arrayIndex();
    fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
}

// Calculate SpeciesImproper forces for specified Atom only
void ForceKernel::forces(const std::shared_ptr<Atom> onlyThis, const SpeciesImproper &imp, const std::shared_ptr<Atom> i,
                         const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l)
{
    // Calculate vectors, ensuring we account for minimum image
    Vec3<double> vecji, vecjk, veckl;
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(i, j);
    else
        vecji = j->r() - i->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(k, j);
    else
        vecjk = j->r() - k->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(l, k);
    else
        veckl = k->r() - l->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(phi_ * DEGRAD);

    // Calculate forces for specified atom
    auto index = onlyThis->arrayIndex();
    if (onlyThis == i)
    {
        fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
        fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
        fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));
    }
    else if (onlyThis == j)
    {
        fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
        fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
        fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                                 dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));
    }
    else if (onlyThis == k)
    {
        fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
        fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
        fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                                 dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));
    }
    else
    {
        fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
        fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
        fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
    }
}

// Calculate SpeciesImproper forces
void ForceKernel::forces(const SpeciesImproper &imp)
{
    // Calculate vectors, ensuring we account for minimum image
    const Vec3<double> vecji = imp.j()->r() - imp.i()->r(), vecjk = imp.j()->r() - imp.k()->r(),
                       veckl = imp.k()->r() - imp.l()->r();

    calculateTorsionParameters(vecji, vecjk, veckl);
    const auto du_dphi = imp.force(phi_ * DEGRAD);

    // Calculate forces on atom i
    auto index = imp.i()->index();
    fx_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpj_.dp(dxpj_dij_.columnAsVec3(2));

    index = imp.j()->index();
    fx_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(0) - dxpj_dkj_.columnAsVec3(0)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(1) - dxpj_dkj_.columnAsVec3(1)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpj_.dp(-dxpj_dij_.columnAsVec3(2) - dxpj_dkj_.columnAsVec3(2)) -
                             dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2)));

    index = imp.k()->index();
    fx_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(0) - dxpk_dlk_.columnAsVec3(0)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(0)));
    fy_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(1) - dxpk_dlk_.columnAsVec3(1)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(1)));
    fz_[index] += du_dphi * (dcos_dxpk_.dp(dxpk_dkj_.columnAsVec3(2) - dxpk_dlk_.columnAsVec3(2)) +
                             dcos_dxpj_.dp(dxpj_dkj_.columnAsVec3(2)));

    index = imp.l()->index();
    fx_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(0));
    fy_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(1));
    fz_[index] += du_dphi * dcos_dxpk_.dp(dxpk_dlk_.columnAsVec3(2));
}
