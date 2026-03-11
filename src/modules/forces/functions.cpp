// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/timer.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "modules/forces/forces.h"

// Calculate total forces within the supplied Configuration
void ForcesModule::totalForces(Configuration *cfg, const PotentialMap &potentialMap, ForceCalculationType calculationType,
                               std::vector<Vector3> &fUnbound, std::vector<Vector3> &fBound)
{
    // Create a Timer
    Timer timer;

    // Zero force arrays
    std::fill(fUnbound.begin(), fUnbound.end(), Vector3());
    if (&fUnbound != &fBound)
        std::fill(fBound.begin(), fBound.end(), Vector3());

    // Create a ForceKernel
    auto kernel = KernelProducer::forceKernel(cfg, potentialMap);

    timer.start();
    if (calculationType == ForceCalculationType::Full)
        kernel->totalForces(fUnbound, fBound);
    else if (calculationType == ForceCalculationType::PairPotentialOnly)
        kernel->totalForces(fUnbound, fBound, {Kernel::ExcludeGeometric, Kernel::ExcludeExtended});
    else if (calculationType == ForceCalculationType::IntraMolecularFull)
        kernel->totalForces(fUnbound, fBound, {Kernel::ExcludeInterMolecularPairPotential, Kernel::ExcludeExtended});
    else if (calculationType == ForceCalculationType::IntraMolecularGeometry)
        kernel->totalForces(
            fUnbound, fBound,
            {Kernel::ExcludeInterMolecularPairPotential, Kernel::ExcludeIntraMolecularPairPotential, Kernel::ExcludeExtended});

    timer.stop();
    Messenger::printVerbose("Time to do forces was {}.\n", timer.totalTimeString());
}

// Calculate forces acting on specific Molecules within the specified Configuration (arising from all atoms)
void ForcesModule::totalForces(Configuration *cfg, const std::vector<const Molecule *> &targetMolecules,
                               const PotentialMap &potentialMap, ForceCalculationType calculationType,
                               std::vector<Vector3> &fUnbound, std::vector<Vector3> &fBound)
{
    std::vector<Vector3> tempFUnbound(fUnbound.size(), Vector3()), tempFBound(fBound.size(), Vector3());
    totalForces(cfg, potentialMap, calculationType, tempFUnbound, tempFBound);

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
