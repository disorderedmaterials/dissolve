// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/force.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
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

// Calculate forces between Cell and its neighbours
void ForceKernel::cellPairPotentialForces(const Cell *cell, bool excludeIgeJ, bool includeIntraMolecular,
                                          ProcessPool::DivisionStrategy strategy, ForceVector &f) const
{
    assert(cellArray_);

    auto &neighbours = cellArray_->get().neighbours(*cell);
    for (auto it = std::next(neighbours.begin()); it != neighbours.end(); ++it)
        cellToCellPairPotentialForces(cell, &it->neighbour_, it->requiresMIM_, excludeIgeJ, includeIntraMolecular, strategy, f);
}

// Calculate forces between atoms in supplied cells
void ForceKernel::cellToCellPairPotentialForces(const Cell *centralCell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                                                bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy,
                                                ForceVector &f) const
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
                else if (includeIntraMolecular)
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
                else if (includeIntraMolecular)
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

/*
 * Extended Terms
 */

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

    // Pair potential forces
    if (!flags.isSet(ExcludePairPotential))
    {
        auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

        // Force operator
        auto unaryOp = [&, strategy](const int id)
        {
            auto *cellI = cellArray.cell(id);
            auto &fLocal = combinableUnbound.local();
            // This cell with itself
            cellToCellPairPotentialForces(cellI, cellI, false, true, !flags.isSet(ExcludeIntraMolecularPairPotential),
                                          ProcessPool::subDivisionStrategy(strategy), fLocal);
            // Interatomic interactions between atoms in this cell and its neighbours
            cellPairPotentialForces(cellI, true, !flags.isSet(ExcludeIntraMolecularPairPotential),
                                    ProcessPool::subDivisionStrategy(strategy), fLocal);
        };

        // Execute lambda operator for each cell
        dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin),
                           dissolve::counting_iterator<int>(end), unaryOp);
    }

    // Other molecule forces
    if (!(flags.isSet(ExcludeGeometry) && flags.isSet(ExcludePairPotential) && flags.isSet(ExcludeExtended)))
    {
        auto moleculeForceOperator = [&](const auto &mol)
        {
            auto &fLocalUnbound = combinableUnbound.local();
            auto &fLocalBound = combinableBound.local();

            // Geometric terms
            if (!flags.isSet(ExcludeGeometry))
                totalGeometryForces(*mol.get(), fLocalBound);

            // Pair potential interactions between atoms within the molecule
            // Only include these terms if we haven't already included them in the cell-to-cell loop above

            if (flags.isSet(ExcludePairPotential) && !flags.isSet(ExcludeIntraMolecularPairPotential))
                dissolve::for_each_pair(ParallelPolicies::seq, mol->atoms().begin(), mol->atoms().end(),
                                        [&](int indexI, const auto &i, int indexJ, const auto &j)
                                        {
                                            if (indexI == indexJ)
                                                return;
                                            auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                                            if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                                forcesWithMim(*i, *j, fLocalUnbound);
                                            else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                                forcesWithMim(*i, *j, fLocalUnbound, elec14, vdw14);
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
