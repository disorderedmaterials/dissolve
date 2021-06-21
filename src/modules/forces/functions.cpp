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
    auto combinableForces = createCombinableForces(f);

    // Set start/stride for parallel loop
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

    // algorithm parameters
    auto unaryOp = [&combinableForces, &kernel, &cellArray, strategy](const int id) {
        auto *cellI = cellArray.cell(id);
        auto &fLocal = combinableForces.local();
        // This cell with itself
        kernel.forces(cellI, cellI, false, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
        // Interatomic interactions between atoms in this cell and its neighbours
        kernel.forces(cellI, true, ProcessPool::subDivisionStrategy(strategy), fLocal);
    };
    // Execute lambda operator for each cell
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    combinableForces.finalize();
}

// Calculate interatomic forces on specified atoms within the specified Configuration
void ForcesModule::interAtomicForces(ProcessPool &procPool, Configuration *cfg, const std::vector<int> &targetIndices,
                                     const PotentialMap &potentialMap, std::vector<Vec3<double>> &f)
{
    /*
     * Calculates the interatomic forces in the specified Configuration arising from contributions from PairPotential
     * interactions between individual atoms, and accounting for intramolecular terms
     *
     * This is a parallel routine, with processes operating as process groups.
     */

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), potentialMap);
    auto combinableForces = createCombinableForces(f);

    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);

    // Loop over supplied atom indices
    auto [begin, end] = chop_range(0, static_cast<int>(targetIndices.size()), stride, start);
    auto unaryOp = [&combinableForces, &kernel, cfg, &targetIndices, strategy](const int n) {
        auto &fLocal = combinableForces.local();
        kernel.forces(*cfg->atoms()[targetIndices.at(n)], ProcessPool::subDivisionStrategy(strategy), fLocal);
    };
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    combinableForces.finalize();
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

// Calculate total intramolecular forces acting on specific atoms in Configuration
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Configuration *cfg, const std::vector<int> &targetIndices,
                                        const PotentialMap &potentialMap, std::vector<Vec3<double>> &f)
{
    /*
     * Calculate the total intramolecular forces for the specific atoms provided, arising from Bond, Angle, and Torsion
     * terms.
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
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);

    // Loop over supplied atom indices
    const auto &atoms = cfg->atoms();
    auto [begin, end] = chop_range(0, static_cast<int>(targetIndices.size()), stride, start);

    auto unaryOp = [&](const auto n) {
        const auto i = atoms[targetIndices.at(n)];
        auto fLocal = combinableForces.local();
        const SpeciesAtom *spAtom = i->speciesAtom();
        auto mol = i->molecule();

        // Calculate forces from SpeciesBond terms
        for (const SpeciesBond &bond : spAtom->bonds())
            kernel.forces(*i, bond, *mol->atom(bond.indexI()), *mol->atom(bond.indexJ()), fLocal);

        // Calculate forces from SpeciesAngle terms
        for (const SpeciesAngle &angle : spAtom->angles())
            kernel.forces(*i, angle, *mol->atom(angle.indexI()), *mol->atom(angle.indexJ()), *mol->atom(angle.indexK()),
                          fLocal);

        // Calculate forces from SpeciesTorsion terms
        for (const SpeciesTorsion &torsion : spAtom->torsions())
            kernel.forces(*i, torsion, *mol->atom(torsion.indexI()), *mol->atom(torsion.indexJ()), *mol->atom(torsion.indexK()),
                          *mol->atom(torsion.indexL()), fLocal);

        // Calculate forces from SpeciesImproper terms
        for (const SpeciesImproper &improper : spAtom->impropers())
            kernel.forces(*i, improper, *mol->atom(improper.indexI()), *mol->atom(improper.indexJ()),
                          *mol->atom(improper.indexK()), *mol->atom(improper.indexL()), fLocal);
    };
    dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(begin), dissolve::counting_iterator<int>(end),
                       unaryOp);
    combinableForces.finalize();
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
            kernel.forces(bond, *mol->atom(bond.indexI()), *mol->atom(bond.indexJ()), fLocal);

        // Loop over angles
        for (const auto &angle : mol->species()->angles())
            kernel.forces(angle, *mol->atom(angle.indexI()), *mol->atom(angle.indexJ()), *mol->atom(angle.indexK()), fLocal);

        // Loop over torsions
        for (const auto &torsion : mol->species()->torsions())
            kernel.forces(torsion, *mol->atom(torsion.indexI()), *mol->atom(torsion.indexJ()), *mol->atom(torsion.indexK()),
                          *mol->atom(torsion.indexL()), fLocal);

        // Loop over impropers
        for (const auto &imp : mol->species()->impropers())
            kernel.forces(imp, *mol->atom(imp.indexI()), *mol->atom(imp.indexJ()), *mol->atom(imp.indexK()),
                          *mol->atom(imp.indexL()), fLocal);
    };
    dissolve::for_each(ParallelPolicies::par, begin, end, unaryOp);
    combinableForces.finalize();
}

// Calculate total intramolecular forces in Species
void ForcesModule::intraMolecularForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                                        std::vector<Vec3<double>> &f)
{
    NonPeriodicBox box(1.0);
    ForceKernel kernel(procPool, &box, potentialMap);

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

// Calculate forces acting on specific atoms within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(ProcessPool &procPool, Configuration *cfg, const std::vector<int> &targetIndices,
                               const PotentialMap &potentialMap, std::vector<Vec3<double>> &f)
{
    /*
     * Calculates the total forces acting on the atom indices in the supplied Configuration, arising from PairPotential
     * interactions and intramolecular contributions from *all other atoms* in the Configuration.
     *
     * The supplied force arrays (fx, fy, and fz) should be initialised to the total number of atoms in the Configuration,
     * rather than the number of atoms in the targetIndices list.
     *
     * This is a serial routine (subroutines called from within are parallel).
     */

    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(f.begin(), f.end(), Vec3<double>());

    // Calculate interatomic forces
    timer.start();
    interAtomicForces(procPool, cfg, targetIndices, potentialMap, f);
    timer.stop();
    Messenger::printVerbose("Time to do partial interatomic forces was {}.\n", timer.totalTimeString());

    // Calculate intramolecular forces
    timer.start();
    intraMolecularForces(procPool, cfg, targetIndices, potentialMap, f);
    timer.stop();
    Messenger::printVerbose("Time to do partial intramolecular forces was {}.\n", timer.totalTimeString());

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

    // Convert the Molecule array into an array of atoms
    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    std::vector<int> indices;
    for (const auto *mol : targetMolecules)
    {
        for (auto i = 0; i < mol->nAtoms(); ++i)
            indices.push_back(mol->atom(i)->arrayIndex());
    }

    // Call the atomic index-based function
    totalForces(procPool, cfg, indices, potentialMap, f);
}

// Calculate total forces within the specified Species
void ForcesModule::totalForces(ProcessPool &procPool, Species *sp, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f)
{
    interAtomicForces(procPool, sp, potentialMap, f);

    intraMolecularForces(procPool, sp, potentialMap, f);
}
