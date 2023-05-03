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
void ForceKernel::cellPairPotentialForces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy,
                                          ForceVector &f) const
{
    assert(cellArray_);

    auto &neighbours = cellArray_->get().neighbours(*cell);
    for (auto it = std::next(neighbours.begin()); it != neighbours.end(); ++it)
        cellToCellPairPotentialForces(cell, &it->neighbour_, it->requiresMIM_, excludeIgeJ, strategy, f);
}

// Calculate forces between atoms in supplied cells
void ForceKernel::cellToCellPairPotentialForces(const Cell *centralCell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
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

// Calculate total pairpotential forces between atoms in the world
void ForceKernel::totalPairPotentialForces(std::vector<Vec3<double>> &f, ProcessPool::DivisionStrategy strategy) const
{
    assert(cellArray_);
    auto &cellArray = cellArray_->get();

    // Create a force combinable
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
        cellToCellPairPotentialForces(cellI, cellI, false, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
        // Interatomic interactions between atoms in this cell and its neighbours
        cellPairPotentialForces(cellI, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
    };

    // Execute lambda operator for each cell
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    combinableForces.finalize();
}

/*
 * Intramolecular Terms
 */

// Calculate total (intramolecular) forces within the specified molecule
void ForceKernel::totalIntramolecularForces(Molecule *mol, ForceVector &f, bool includePairPotentialTerms) const
{
    totalGeometryForces(mol, f);

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

        totalIntramolecularForces(mol.get(), fLocal, includePairPotentialTerms);
    };

    // Set start/stride for parallel loop
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(molecules.begin(), molecules.end(), stride, start);
    dissolve::for_each(ParallelPolicies::par, begin, end, moleculeForceOperator);

    combinable.finalize();
}
