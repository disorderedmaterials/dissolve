// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/force.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>

ForceKernel::ForceKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                         std::optional<double> energyCutoff)
    : GeometryKernel(cfg, procPool, potentialMap, energyCutoff)
{
}

ForceKernel::ForceKernel(const Box *box, const ProcessPool &procPool, const PotentialMap &potentialMap,
                         std::optional<double> energyCutoff)
    : GeometryKernel(box, procPool, potentialMap, energyCutoff)
{
}

/*
 * Force Calculation
 */

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithoutMim(const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f) const
{
    auto vij = j.r() - i.r();
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithoutMim(const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f, double elecScale,
                                   double vdwScale) const
{
    auto vij = j.r() - i.r();
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r, elecScale, vdwScale);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithMim(const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f) const
{
    auto vij = box_->minimumVector(i.r(), j.r());
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithMim(const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f, double elecScale,
                                double vdwScale) const
{
    auto vij = box_->minimumVector(i.r(), j.r());
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r, elecScale, vdwScale);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms in supplied cells
void ForceKernel::cellToCellPairPotentialForces(const Cell *centralCell, const Cell *otherCell, bool applyMim,
                                                ForceVector &f) const
{
    assert(centralCell && otherCell);
    auto &centralAtoms = centralCell->atoms();
    auto &otherAtoms = otherCell->atoms();
    std::shared_ptr<Molecule> molI;

    // Loop over all atom pairs excluding any within the same molecule
    if (applyMim)
    {
        for (const auto &i : centralAtoms)
        {
            molI = i->molecule();
            auto indexI = molI->globalAtomIndex(i);

            for (auto *j : otherAtoms)
                if (molI != j->molecule())
                    forcesWithMim(*i, indexI, *j, j->molecule()->globalAtomIndex(j), f);
        }
    }
    else
    {
        for (const auto &i : centralAtoms)
        {
            molI = i->molecule();
            auto indexI = molI->globalAtomIndex(i);

            for (auto *j : otherAtoms)
                if (molI != j->molecule())
                    forcesWithoutMim(*i, indexI, *j, j->molecule()->globalAtomIndex(j), f);
        }
    }
}

/*
 * Extended Terms
 */

// Calculate extended forces on supplied atom
void ForceKernel::extendedForces(const Atom &i, Vec3<double> &fVec) const { return; }

// Calculate extended forces on supplied molecule
void ForceKernel::extendedForces(const Molecule &mol, ForceVector &f) const { return; }

/*
 * Totals
 */

// Calculate total forces in the world
void ForceKernel::totalForces(ForceVector &fUnbound, ForceVector &fBound, ProcessPool::DivisionStrategy strategy,
                              Flags<ForceCalculationFlags> flags) const
{
    assert(molecules_);
    assert(cellArray_);

    auto &molecules = molecules_->get();
    auto &cellArray = cellArray_->get();

    // Set start/stride for parallel loop
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);

    auto combinableUnbound = createCombinableForces(fUnbound);
    auto combinableBound = createCombinableForces(fBound);

    // Pair potential forces between different molecules
    if (!flags.isSet(ExcludeInterMolecularPairPotential))
    {
        auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

        // Force operator
        auto unaryOp = [&](const int id)
        {
            auto *cellI = cellArray.cell(id);
            auto &fLocal = combinableUnbound.local();

            // Interatomic interactions between atoms in this cell, excluding those within the same molecule
            dissolve::for_each_pair(ParallelPolicies::seq, cellI->atoms().begin(), cellI->atoms().end(),
                                    [&](int indexI, const auto &i, int indexJ, const auto &j)
                                    {
                                        if (indexI == indexJ)
                                            return;
                                        // Check for atoms in the same molecule
                                        if (i->molecule() != j->molecule())
                                            forcesWithoutMim(*i, i->globalIndex(), *j, j->globalIndex(), fLocal);
                                    });

            // Interatomic interactions between atoms in this cell and its neighbours
            auto &neighbours = cellArray_->get().neighbours(*cellI);
            for (auto it = std::next(neighbours.begin()); it != neighbours.end(); ++it)
            {
                if (it->neighbour_.index() < cellI->index())
                    cellToCellPairPotentialForces(cellI, &it->neighbour_, it->requiresMIM_, fLocal);
            }
        };

        // Execute lambda operator for each cell
        dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin),
                           dissolve::counting_iterator<int>(end), unaryOp);
    }

    // Other molecule forces
    if (!(flags.isSet(ExcludeGeometry) && flags.isSet(ExcludeIntraMolecularPairPotential) && flags.isSet(ExcludeExtended)))
    {
        auto moleculeForceOperator = [&](const auto &mol)
        {
            auto &fLocalUnbound = combinableUnbound.local();
            auto &fLocalBound = combinableBound.local();

            auto offset = mol->globalAtomOffset();

            // Geometric terms
            if (!flags.isSet(ExcludeGeometry))
                totalGeometryForces(*mol.get(), fLocalBound);

            // Pair potential interactions between atoms within the molecule
            if (!flags.isSet(ExcludeIntraMolecularPairPotential))
                dissolve::for_each_pair(ParallelPolicies::seq, mol->atoms().begin(), mol->atoms().end(),
                                        [&](int indexI, const auto &i, int indexJ, const auto &j)
                                        {
                                            if (indexI == indexJ)
                                                return;
                                            auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                                            if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                                forcesWithMim(*i, offset + indexI, *j, offset + indexJ, fLocalUnbound);
                                            else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                                forcesWithMim(*i, offset + indexI, *j, offset + indexJ, fLocalUnbound, elec14,
                                                              vdw14);
                                        });

            // Extended forces
            if (!flags.isSet(ExcludeExtended))
                extendedForces(*mol.get(), fLocalUnbound);
        };

        auto [begin, end] = chop_range(molecules.begin(), molecules.end(), stride, start);
        dissolve::for_each(ParallelPolicies::par, begin, end, moleculeForceOperator);
    }

    combinableUnbound.finalize();
    combinableBound.finalize();
}
