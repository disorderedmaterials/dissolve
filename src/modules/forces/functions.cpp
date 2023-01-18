// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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

// Calculate internal molecule forces, optionally only over the supplied molecules
void ForcesModule::internalMoleculeForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                          bool includePairPotentialTerms, std::vector<Vec3<double>> &fInter,
                                          std::vector<Vec3<double>> &fIntra,
                                          OptionalReferenceWrapper<std::vector<const Molecule *>> targetMolecules)
{
    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg->box(), cfg->cells(), potentialMap);

    // Molecule Force Operator
    auto combinableForcesInter = createCombinableForces(fInter);
    auto combinableForcesIntra = createCombinableForces(fIntra);
    auto moleculeForceOperator =
        [&combinableForcesInter, &combinableForcesIntra, &kernel, includePairPotentialTerms](const auto &mol)
    {
        auto &fLocalInter = combinableForcesInter.local();
        auto &fLocalIntra = combinableForcesIntra.local();

        // Loop over bonds
        for (const auto &bond : mol->species()->bonds())
            kernel.forces(bond, *mol->atom(bond.indexI()), *mol->atom(bond.indexJ()), fLocalIntra);

        // Loop over angles
        for (const auto &angle : mol->species()->angles())
            kernel.forces(angle, *mol->atom(angle.indexI()), *mol->atom(angle.indexJ()), *mol->atom(angle.indexK()),
                          fLocalIntra);

        // Loop over torsions
        for (const auto &torsion : mol->species()->torsions())
            kernel.forces(torsion, *mol->atom(torsion.indexI()), *mol->atom(torsion.indexJ()), *mol->atom(torsion.indexK()),
                          *mol->atom(torsion.indexL()), fLocalIntra);

        // Loop over impropers
        for (const auto &imp : mol->species()->impropers())
            kernel.forces(imp, *mol->atom(imp.indexI()), *mol->atom(imp.indexJ()), *mol->atom(imp.indexK()),
                          *mol->atom(imp.indexL()), fLocalIntra);

        // Pair potential interactions between atoms
        if (includePairPotentialTerms)
            dissolve::for_each_pair(ParallelPolicies::seq, mol->atoms().begin(), mol->atoms().end(),
                                    [&](int indexI, const auto &i, int indexJ, const auto &j)
                                    {
                                        if (indexI == indexJ)
                                            return;
                                        auto scale = i->scaling(j);
                                        if (scale >= 1.0e-3)
                                            kernel.forcesWithMim(*i, *j, fLocalInter, scale);
                                    });
    };

    // Set start/stride for parallel loop
    auto start = procPool.interleavedLoopStart(ProcessPool::PoolStrategy);
    auto stride = procPool.interleavedLoopStride(ProcessPool::PoolStrategy);
    if (targetMolecules)
    {
        auto [begin, end] = chop_range(targetMolecules->get().begin(), targetMolecules->get().end(), stride, start);
        dissolve::for_each(ParallelPolicies::par, begin, end, moleculeForceOperator);
    }
    else
    {
        auto [begin, end] = chop_range(cfg->molecules().begin(), cfg->molecules().end(), stride, start);
        dissolve::for_each(ParallelPolicies::par, begin, end, moleculeForceOperator);
    }

    combinableForcesInter.finalize();
    combinableForcesIntra.finalize();
}
void ForcesModule::internalMoleculeForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                                          bool includePairPotentialTerms, std::vector<Vec3<double>> &f,
                                          OptionalReferenceWrapper<std::vector<const Molecule *>> targetMolecules)
{
    internalMoleculeForces(procPool, cfg, potentialMap, includePairPotentialTerms, f, f, targetMolecules);
}

// Calculate pair potential forces within the specified Configuration
void ForcesModule::pairPotentialForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
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
    const auto kernel = ForceKernel(procPool, cfg->box(), cfg->cells(), potentialMap);
    auto combinableForces = createCombinableForces(f);

    // Set start/stride for parallel loop
    ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;
    auto start = procPool.interleavedLoopStart(strategy);
    auto stride = procPool.interleavedLoopStride(strategy);
    auto [begin, end] = chop_range(0, cellArray.nCells(), stride, start);

    // algorithm parameters
    auto unaryOp = [&combinableForces, &kernel, &cellArray, strategy](const int id)
    {
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

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra,
                               OptionalReferenceWrapper<Timer> commsTimer)
{
    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(fInter.begin(), fInter.end(), Vec3<double>());
    std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());

    // Calculate pair potential forces between all atoms (including within molecules)
    timer.start();
    pairPotentialForces(procPool, cfg, potentialMap, fInter);
    timer.stop();
    Messenger::printVerbose("Time to do pair potential forces was {}.\n", timer.totalTimeString());

    // Calculate internal molecule forces (excluding pair potential terms)
    timer.start();
    internalMoleculeForces(procPool, cfg, potentialMap, false, fIntra);
    timer.stop();
    Messenger::printVerbose("Time to do internal molecule forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    procPool.allSum(fInter, ProcessPool::PoolProcessesCommunicator, commsTimer);
    if (&fInter != &fIntra)
        procPool.allSum(fIntra, ProcessPool::PoolProcessesCommunicator, commsTimer);
}
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f, OptionalReferenceWrapper<Timer> commsTimer)
{
    totalForces(procPool, cfg, potentialMap, f, f, commsTimer);
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg,
                               const std::vector<const Molecule *> &targetMolecules, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra,
                               OptionalReferenceWrapper<Timer> commsTimer)
{
    std::vector<Vec3<double>> tempFInter(fInter.size(), Vec3<double>()), tempFIntra(fIntra.size(), Vec3<double>());
    std::fill(tempFInter.begin(), tempFInter.end(), Vec3<double>());
    std::fill(tempFIntra.begin(), tempFIntra.end(), Vec3<double>());
    totalForces(procPool, cfg, potentialMap, tempFInter, tempFIntra, commsTimer);

    // Convert the Molecule array into an array of atoms
    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    std::vector<int> indices;
    for (const auto *mol : targetMolecules)
        for (const auto &i : mol->atoms())
        {
            fInter[i->arrayIndex()] = tempFInter[i->arrayIndex()];
            fIntra[i->arrayIndex()] = tempFIntra[i->arrayIndex()];
        }
}
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg,
                               const std::vector<const Molecule *> &targetMolecules, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f, OptionalReferenceWrapper<Timer> commsTimer)
{
    std::vector<Vec3<double>> tempf(f.size(), Vec3<double>());
    std::fill(f.begin(), f.end(), Vec3<double>());
    totalForces(procPool, cfg, potentialMap, tempf, commsTimer);

    // Convert the Molecule array into an array of atoms
    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    std::vector<int> indices;
    for (const auto *mol : targetMolecules)
        for (const auto &i : mol->atoms())
            f[i->arrayIndex()] = tempf[i->arrayIndex()];
}

// Calculate total forces within the specified Species
void ForcesModule::totalForces(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &f)
{
    // Zero force array
    std::fill(f.begin(), f.end(), Vec3<double>());

    auto *box = sp->box();
    const auto cutoffSq = potentialMap.range() * potentialMap.range();

    auto combinableForces = createCombinableForces(f);
    auto pairwiseForceOperator =
        [&combinableForces, &potentialMap, cutoffSq, box](int indexI, const auto &i, int indexJ, const auto &j)
    {
        if (indexI == indexJ)
            return;
        auto scale = i.scaling(&j);
        if (scale >= 1.0e-3)
        {
            // Determine final forces
            auto vecij = box->minimumVector(j.r(), i.r());
            auto magjisq = vecij.magnitudeSq();
            if (magjisq <= cutoffSq)
            {
                auto r = sqrt(magjisq);
                vecij /= r;

                vecij *= potentialMap.force(&i, &j, r) * scale;
                auto &fLocal = combinableForces.local();
                fLocal[indexI] += vecij;
                fLocal[indexJ] -= vecij;
            }
        }
    };

    // Calculate pairwise forces between atoms
    if (sp->nAtoms() < 100)
        dissolve::for_each_pair(ParallelPolicies::seq, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);
    else
        dissolve::for_each_pair(ParallelPolicies::par, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);

    combinableForces.finalize();

    // Create a ForceKernel with a dummy CellArray
    CellArray dummyCellArray;
    ForceKernel kernel(procPool, sp->box(), dummyCellArray, potentialMap);

    // Loop over bonds
    for (const auto &b : sp->bonds())
        kernel.forces(b, b.i()->r(), b.j()->r(), f);

    // Loop over angles
    for (const auto &a : sp->angles())
        kernel.forces(a, a.i()->r(), a.j()->r(), a.k()->r(), f);

    // Loop over torsions
    for (const auto &t : sp->torsions())
        kernel.forces(t, t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r(), f);

    // Loop over impropers
    for (const auto &imp : sp->impropers())
        kernel.forces(imp, imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r(), f);
}

// Calculate total forces within the specified Species,
void ForcesModule::totalForces(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap,
                               const std::vector<Vec3<double>> &r, std::vector<Vec3<double>> &f)
{
    assert(sp->nAtoms() == r.size());

    // Zero force array
    std::fill(f.begin(), f.end(), Vec3<double>());

    double scale, rij, magjisq;
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
            vecij = r[indexJ] - r[indexI];
            magjisq = vecij.magnitudeSq();
            if (magjisq > cutoffSq)
                continue;
            rij = sqrt(magjisq);
            vecij /= rij;

            vecij *= potentialMap.force(&i, &j, rij) * scale;
            f[indexI] += vecij;
            f[indexJ] -= vecij;
        }
    }

    // Create a ForceKernel with a dummy CellArray - we only want it for the intramolecular force routines
    CellArray dummyCellArray;
    ForceKernel kernel(procPool, sp->box(), dummyCellArray, potentialMap);

    // Loop over bonds
    for (const auto &b : sp->bonds())
        kernel.forces(b, r[b.i()->index()], r[b.j()->index()], f);

    // Loop over angles
    for (const auto &a : sp->angles())
        kernel.forces(a, r[a.i()->index()], r[a.j()->index()], r[a.k()->index()], f);

    // Loop over torsions
    for (const auto &t : sp->torsions())
        kernel.forces(t, r[t.i()->index()], r[t.j()->index()], r[t.k()->index()], r[t.l()->index()], f);

    // Loop over impropers
    for (const auto &imp : sp->impropers())
        kernel.forces(imp, r[imp.i()->index()], r[imp.j()->index()], r[imp.k()->index()], r[imp.l()->index()], f);
}
