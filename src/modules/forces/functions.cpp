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

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(const ProcessPool &procPool, Configuration *cfg, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra,
                               OptionalReferenceWrapper<Timer> commsTimer)
{
    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(fInter.begin(), fInter.end(), Vec3<double>());
    if (&fInter != &fIntra)
        std::fill(fIntra.begin(), fIntra.end(), Vec3<double>());

    // Create a ForceKernel
    ForceKernel kernel(procPool, cfg, potentialMap);

    // Calculate pair potential forces between all atoms (including within molecules)
    timer.start();
    kernel.totalPairPotentialForces(fInter, ProcessPool::PoolStrategy);
    timer.stop();
    Messenger::printVerbose("Time to do pair potential forces was {}.\n", timer.totalTimeString());

    // Calculate internal molecule forces (excluding pair potential terms)
    timer.start();
    kernel.totalIntramolecularForces(fIntra, false, ProcessPool::PoolStrategy);
    timer.stop();
    Messenger::printVerbose("Time to do internal molecule forces was {}.\n", timer.totalTimeString());

    // Gather forces together over all processes
    procPool.allSum(fInter, ProcessPool::PoolProcessesCommunicator, commsTimer);
    if (&fInter != &fIntra)
        procPool.allSum(fIntra, ProcessPool::PoolProcessesCommunicator, commsTimer);
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

// Calculate total forces within the specified Species
void ForcesModule::totalForces(const ProcessPool &procPool, const Species *sp, const PotentialMap &potentialMap,
                               std::vector<Vec3<double>> &fUnbound, std::vector<Vec3<double>> &fBound,
                               OptionalReferenceWrapper<const std::vector<Vec3<double>>> r)
{
    if (r)
        assert(sp->nAtoms() == r.size());

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
    if (sp->nAtoms() < 100)
        dissolve::for_each_pair(ParallelPolicies::seq, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);
    else
        dissolve::for_each_pair(ParallelPolicies::par, sp->atoms().begin(), sp->atoms().end(), pairwiseForceOperator);

    combinableUnbound.finalize();

    // Create a ForceKernel with a dummy CellArray - we only want it for the intramolecular force routines
    ForceKernel kernel(procPool, sp->box(), potentialMap);

    // Loop over bonds
    for (const auto &b : sp->bonds())
        kernel.forces(b, rFunction(b.i()->index(), b.i()), rFunction(b.j()->index(), b.j()), fBound);

    // Loop over angles
    for (const auto &a : sp->angles())
        kernel.forces(a, rFunction(a.i()->index(), a.i()), rFunction(a.j()->index(), a.j()), rFunction(a.k()->index(), a.k()),
                      fBound);

    // Loop over torsions
    for (const auto &t : sp->torsions())
        kernel.forces(t, rFunction(t.i()->index(), t.i()), rFunction(t.j()->index(), t.j()), rFunction(t.k()->index(), t.k()),
                      rFunction(t.l()->index(), t.l()), fBound);

    // Loop over impropers
    for (const auto &imp : sp->impropers())
        kernel.forces(imp, rFunction(imp.i()->index(), imp.i()), rFunction(imp.j()->index(), imp.j()),
                      rFunction(imp.k()->index(), imp.k()), rFunction(imp.l()->index(), imp.l()), fBound);
}
