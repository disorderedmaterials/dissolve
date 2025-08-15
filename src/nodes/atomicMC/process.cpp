// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "base/timer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include "nodes/atomicMC/atomicMC.h"

// Run main processing
NodeConstants::ProcessResult AtomicMCNode::process()
{
    // Get numeric input data
    auto nShakesPerAtom = nShakesPerAtom_.asInteger();
    auto stepSize = stepSize_.asDouble();
    auto stepSizeMax = stepSizeMax_.asDouble();
    auto stepSizeMin = stepSizeMin_.asDouble();
    auto targetAcceptanceRate = targetAcceptanceRate_.asDouble();

    // Retrieve control parameters from Configuration
    const auto termScale = 1.0;
    const auto rRT = 1.0 / (.008314472 * targetConfiguration_->temperature());

    // Print argument/parameter summary
    message("Performing {} shake(s) per Atom\n", nShakesPerAtom);
    message("Step size for adjustments is {:.5f} Angstroms (allowed range is {} <= delta <= {}).\n", stepSize, stepSizeMin,
            stepSizeMax);
    message("Target acceptance rate is {}.\n", targetAcceptanceRate);
    message("\n");

    auto kernel =
        KernelProducer::energyKernel(targetConfiguration_, dissolve().potentialMap(), dissolve().pairPotentialRange());

    auto nAttempts = 0, nAccepted = 0;
    bool accept;
    double currentEnergy, currentIntraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
    Vector3 rDelta;
    EnergyResult er;

    Timer timer;
    // Loop over target Molecules
    for (auto mol : targetConfiguration_->molecules())
    {
        /*
         * Calculation Begins
         */

        // Loop over atoms in the Molecule
        for (const auto &i : mol->atoms())
        {
            // Calculate reference energies for the Atom
            er = kernel->totalEnergy(*i);
            currentEnergy = er.totalUnbound();
            currentIntraEnergy = er.geometry() * termScale;

            // Loop over number of shakes per Atom
            for (auto n = 0; n < nShakesPerAtom; ++n)
            {
                auto moveInitialPos = i->r();

                // Create a random translation vector
                rDelta.set(DissolveMath::randomPlusMinusOne() * stepSize, DissolveMath::randomPlusMinusOne() * stepSize,
                           DissolveMath::randomPlusMinusOne() * stepSize);

                // Translate Atom and update its Cell position
                i->translateCoordinates(rDelta);
                targetConfiguration_->updateAtomLocation(i);

                // Calculate new energy
                er = kernel->totalEnergy(*i);
                newEnergy = er.totalUnbound();
                newIntraEnergy = er.geometry() * termScale;

                // Trial the transformed Atom position
                delta = (newEnergy + newIntraEnergy) - (currentEnergy + currentIntraEnergy);
                accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));

                // Increase attempt counters
                if (accept)
                {
                    totalDelta += delta;
                    ++nAccepted;
                }
                else
                {
                    // Move not accepted - revert to initial position
                    i->setCoordinates(moveInitialPos);
                    targetConfiguration_->updateAtomLocation(i);
                }
                ++nAttempts;
            }
        }

        /*
         * Calculation End
         */
    }

    timer.stop();

    message("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

    // Calculate and print acceptance rate
    double rate = double(nAccepted) / nAttempts;
    message("Total number of attempted moves was {} ({})\n", nAttempts, timer.totalTimeString());

    message("Overall acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rate, nAccepted, nAttempts);

    // Update and set translation step size
    stepSize *= (nAccepted == 0) ? 0.8 : rate / targetAcceptanceRate;
    if (stepSize < stepSizeMin)
        stepSize = stepSizeMin;
    else if (stepSize > stepSizeMax)
        stepSize = stepSizeMax;

    message("Updated step size is {} Angstroms.\n", stepSize);

    // Increase contents version in Configuration
    if (nAccepted > 0)
        targetConfiguration_->notifyAtomicPositionsChanged();

    return NodeConstants::ProcessResult::Success;
}
