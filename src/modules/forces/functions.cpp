// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "modules/forces/forces.h"

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                               ForceCalculationType calculationType, std::vector<Vec3<double>> &fUnbound,
                               std::vector<Vec3<double>> &fBound, OptionalReferenceWrapper<Timer> commsTimer)
{
    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(fUnbound.begin(), fUnbound.end(), Vec3<double>());
    if (&fUnbound != &fBound)
        std::fill(fBound.begin(), fBound.end(), Vec3<double>());

    // Create a ForceKernel
    auto kernel = KernelProducer::forceKernel(cfg, procPool, potentialMap);

    timer.start();
    if (calculationType == ForceCalculationType::Full)
        kernel->totalForces(fUnbound, fBound, ProcessPool::PoolStrategy);
    else if (calculationType == ForceCalculationType::PairPotentialOnly)
        kernel->totalForces(fUnbound, fBound, ProcessPool::PoolStrategy,
                            {ForceKernel::ExcludeGeometry, ForceKernel::ExcludeExtended});
    else if (calculationType == ForceCalculationType::IntraMolecularFull)
        kernel->totalForces(fUnbound, fBound, ProcessPool::PoolStrategy,
                            {ForceKernel::ExcludeInterMolecularPairPotential, ForceKernel::ExcludeExtended});
    else if (calculationType == ForceCalculationType::IntraMolecularGeometry)
        kernel->totalForces(fUnbound, fBound, ProcessPool::PoolStrategy,
                            {ForceKernel::ExcludeInterMolecularPairPotential, ForceKernel::ExcludeIntraMolecularPairPotential,
                             ForceKernel::ExcludeExtended});

    timer.stop();
    Messenger::printVerbose("Time to do forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    procPool.allSum(fUnbound, ProcessPool::PoolProcessesCommunicator, commsTimer);
    if (&fUnbound != &fBound)
        procPool.allSum(fBound, ProcessPool::PoolProcessesCommunicator, commsTimer);
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg,
                               const std::vector<const Molecule *> &targetMolecules, const PotentialMap &potentialMap,
                               ForceCalculationType calculationType, std::vector<Vec3<double>> &fUnbound,
                               std::vector<Vec3<double>> &fBound, OptionalReferenceWrapper<Timer> commsTimer)
{
    std::vector<Vec3<double>> tempFUnbound(fUnbound.size(), Vec3<double>()), tempFBound(fBound.size(), Vec3<double>());
    totalForces(procPool, cfg, potentialMap, calculationType, tempFUnbound, tempFBound, commsTimer);

    // TODO Calculating forces for whole molecule at once may be more efficient
    // TODO Partitioning atoms of target molecules into cells and running a distributor may be more efficient
    std::vector<int> indices;
    for (const auto *mol : targetMolecules)
        for (const auto &i : mol->atoms())
        {
            fUnbound[i->globalIndex()] = tempFUnbound[i->globalIndex()];
            fBound[i->globalIndex()] = tempFBound[i->globalIndex()];
        }
}

// Calculate total forces within the specified Species
void ForcesModule::totalForces(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap,
                               ForceCalculationType calculationType, std::vector<Vec3<double>> &fUnbound,
                               std::vector<Vec3<double>> &fBound, OptionalReferenceWrapper<const std::vector<Vec3<double>>> r)
{
    if (r)
        assert(sp->nAtoms() == r->get().size());

    // Set position retrieval function
    std::function<Vec3<double>(int, const SpeciesAtom *)> rFunction;
    if (r)
        rFunction = [&r, sp](int id, const SpeciesAtom *i) { return r->get()[id]; };
    else
        rFunction = [sp](int id, const SpeciesAtom *i) { return i->r(); };

    // Zero force arrays
    std::fill(fUnbound.begin(), fUnbound.end(), Vec3<double>());
    std::fill(fBound.begin(), fBound.end(), Vec3<double>());

    auto *box = sp->box();
    const auto cutoffSq = potentialMap.range() * potentialMap.range();
    auto combinableUnbound = ForceKernel::createCombinableForces(fUnbound);

    auto pairwiseForceOperator =
        [&combinableUnbound, rFunction, &potentialMap, cutoffSq, box](int indexI, const auto &i, int indexJ, const auto &j)
    {
        if (indexI == indexJ)
            return;

        auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
        if (scalingType == SpeciesAtom::ScaledInteraction::Excluded)
            return;

        // Determine final forces
        auto vecij = box->minimumVector(rFunction(indexI, &i), rFunction(indexJ, &j));
        auto magjisq = vecij.magnitudeSq();
        if (magjisq > cutoffSq)
            return;

        auto r = sqrt(magjisq);
        vecij /= r;

        if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
            vecij *= potentialMap.force(&i, &j, r);
        else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
            vecij *= potentialMap.force(&i, &j, r, elec14, vdw14);

        auto &fLocal = combinableUnbound.local();
        fLocal[indexI] += vecij;
        fLocal[indexJ] -= vecij;
    };

    // Calculate pairwise forces between atoms
    if (calculationType == ForceCalculationType::Full || calculationType == ForceCalculationType::PairPotentialOnly)
    {
        if (sp->nAtoms() < 100)
            dissolve::for_each_pair(ParallelPolicies::seq, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);
        else
            dissolve::for_each_pair(ParallelPolicies::par, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);
        combinableUnbound.finalize();
    }

    if (calculationType != ForceCalculationType::PairPotentialOnly)
    {
        // Create a ForceKernel with a dummy CellArray - we only want it for the intramolecular force routines
        auto kernel = KernelProducer::forceKernel(sp->box(), procPool, potentialMap);

        // Loop over bonds
        for (const auto &b : sp->bonds())
            kernel->bondForces(b, rFunction(b.i()->index(), b.i()), rFunction(b.j()->index(), b.j()), fBound);

        // Loop over angles
        for (const auto &a : sp->angles())
            kernel->angleForces(a, rFunction(a.i()->index(), a.i()), rFunction(a.j()->index(), a.j()),
                                rFunction(a.k()->index(), a.k()), fBound);

        // Loop over torsions
        for (const auto &t : sp->torsions())
            kernel->torsionForces(t, rFunction(t.i()->index(), t.i()), rFunction(t.j()->index(), t.j()),
                                  rFunction(t.k()->index(), t.k()), rFunction(t.l()->index(), t.l()), fBound);

        // Loop over impropers
        for (const auto &imp : sp->impropers())
            kernel->improperForces(imp, rFunction(imp.i()->index(), imp.i()), rFunction(imp.j()->index(), imp.j()),
                                   rFunction(imp.k()->index(), imp.k()), rFunction(imp.l()->index(), imp.l()), fBound);
    }
}
