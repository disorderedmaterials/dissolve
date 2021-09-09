// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/forcekernel.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "modules/forces/forces.h"
#include "templates/algorithms.h"
#include "templates/combinable.h"

namespace
{
dissolve::CombinableContainer<std::vector<Vec3<double>>> createCombinableForces(std::vector<Vec3<double>> &parentForces)
{
    return {parentForces, [&]() { return std::vector<Vec3<double>>(parentForces.size()); }};
}
} // namespace

// Calculate interatomic forces within the supplied Configuration
void ForcesModule::interAtomicForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                     std::vector<Vec3<double>> &f)
{
    /*
     * Calculates the interatomic forces in the supplied Configuration arising from contributions from PairPotential
     * interactions between individual atoms, and accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Grab the Cell array
    const auto &cellArray = cfg->cells();
    // Create a ForceKernel
    const auto kernel = ForceKernel(procPool, cfg->box(), potentialMap);
    // auto combinableForces = createCombinableForces(f);

    // Set start/stride for parallel loop
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

    // algorithm parameters
    auto unaryOp = [&f, &kernel, &cellArray, strategy](const int id) {
        auto *cellI = cellArray.cell(id);
        auto &fLocal = f; // combinableForces.local();
        // This cell with itself
        kernel.forces(cellI, cellI, false, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
        // Interatomic interactions between atoms in this cell and its neighbours
        kernel.forces(cellArray, cellI, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
    };
    // Execute lambda operator for each cell
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    // combinableForces.finalize();
}

// Calculate interatomic forces within the specified Species
void ForcesModule::interAtomicForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                     std::vector<Vec3<double>> &f)
{
    double scale, r, magjisq;
    const auto cutoffSq = potentialMap.range() * potentialMap.range();
    Vec3<double> vecij;
    // NOTE PR #334 : use for_each_pair
    for (auto indexI = 0; indexI < sp->nAtoms() - 1; ++indexI)
    {
        auto &i = sp->atom(indexI);

        for (auto indexJ = indexI + 1; indexJ < sp->nAtoms(); ++indexJ)
        {
            auto &j = sp->atom(indexJ);

            // Get intramolecular scaling of atom pair
            scale = i.scaling(&j);
            if (scale < 1.0e-3)
                continue;

            // Determine final forces
            vecij = j.r() - i.r();
            magjisq = vecij.magnitudeSq();
            if (magjisq > cutoffSq)
                continue;
            r = sqrt(magjisq);
            vecij /= r;

            vecij *= potentialMap.force(&i, &j, r) * scale;
            f[indexI] += vecij;
            f[indexJ] -= vecij;
        }
    }
}

// Calculate total intramolecular forces in Configuration
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                        std::vector<Vec3<double>> &f)
{
    /*
     * Calculate the total intramolecular forces within the supplied Configuration, arising from Bond, Angle, and Torsion
     * terms in all molecules.
     *
     * Calculated forces are added in to the provided arrays. Assembly of the arrays over processes must be performed by the
     * calling function.
     *
     * This is a parallel routine.
     */

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap);
    auto combinableForces = createCombinableForces(f);

    // Set start/stride for parallel loop
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);
    auto [begin, end] = chop_range(cfg->molecules().begin(), cfg->molecules().end(), stride, start);

    // algorithm parameters
    auto &molecules = cfg->molecules();
    auto unaryOp = [&combinableForces, &kernel, &molecules, strategy](const auto &mol) {
        auto &fLocal = combinableForces.local();
        // Loop over bonds
        for (const auto &bond : mol->species()->bonds())
            kernel.forces(bond, mol->atom(bond.indexI()), mol->atom(bond.indexJ()), fLocal);

        // Loop over angles
        for (const auto &angle : mol->species()->angles())
            kernel.forces(angle, mol->atom(angle.indexI()), mol->atom(angle.indexJ()), mol->atom(angle.indexK()), fLocal);

        // Loop over torsions
        for (const auto &torsion : mol->species()->torsions())
            kernel.forces(torsion, mol->atom(torsion.indexI()), mol->atom(torsion.indexJ()), mol->atom(torsion.indexK()),
                          mol->atom(torsion.indexL()), fLocal);

        // Loop over impropers
        for (const auto &imp : mol->species()->impropers())
            kernel.forces(imp, mol->atom(imp.indexI()), mol->atom(imp.indexJ()), mol->atom(imp.indexK()),
                          mol->atom(imp.indexL()), fLocal);
    };
    dissolve::for_each(ParallelPolicies::par, begin, end, unaryOp);
    combinableForces.finalize();
}

// Calculate total intramolecular forces in Species
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                        std::vector<Vec3<double>> &f)
{
    // Create a ForceKernel with a dummy CellArray - we only want it for the intramolecular force routines
    ForceKernel kernel(procPool, sp->box(), potentialMap);

    // Loop over bonds
    for (const auto &b : sp->bonds())
        kernel.forces(b, f);

    // Loop over angles
    for (const auto &a : sp->angles())
        kernel.forces(a, f);

    // Loop over torsions
    for (const auto &t : sp->torsions())
        kernel.forces(t, f);

    // Loop over impropers
    for (const auto &imp : sp->impropers())
        kernel.forces(imp, f);
}

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f)
{
    /*
     * Calculates the total forces within the supplied Configuration, arising from PairPotential interactions
     * and intramolecular contributions.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(f.begin(), f.end(), Vec3<double>());

    // Calculate interatomic forces
    timer.start();
    interAtomicForces(procPool, cfg, potentialMap, f);
    timer.stop();
    Messenger::printVerbose("Time to do interatomic forces was {}.\n", timer.totalTimeString());

    // Calculate intramolecular forces
    timer.start();
    intraMolecularForces(procPool, cfg, potentialMap, f);
    timer.stop();
    Messenger::printVerbose("Time to do intramolecular forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    if (!procPool.allSum(f))
        return;
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg, const std::vector<const Molecule *> &targetMolecules,
                               const PotentialMap &potentialMap, std::vector<Vec3<double>> &f)
{
    /*
     * Calculates the total forces acting on the supplied Molecules, arising from PairPotential interactions
     * and intramolecular contributions from *all other atoms* in the Configuration.
     *
     * The supplied force arrays (fx, fy, and fz) should be initialised to the total number of atoms in the configuration,
     * rather than the number of atoms in the targetIndices list.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Create a temporary
    std::vector<Vec3<double>> tempf(f.size(), Vec3<double>());
    std::fill(f.begin(), f.end(), Vec3<double>());
    totalForces(procPool, cfg, potentialMap, tempf);

    // Convert the Molecule array into an array of atoms
    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    std::vector<int> indices;
    for (const auto *mol : targetMolecules)
        for (const auto &i : mol->atoms())
            f[i.arrayIndex()-1] = tempf[i.arrayIndex()-1];
}

// Calculate total forces within the specified Species
void ForcesModule::totalForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f)
{
    interAtomicForces(procPool, sp, potentialMap, f);

    intraMolecularForces(procPool, sp, potentialMap, f);
}
