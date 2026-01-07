// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/timer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include "nodes/atomicMC/atomicMC.h"
#include "nodes/dissolve.h"

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

    // Prepare for energy calculation, generate kernel
    auto kernel = dissolveGraph()->prepareEnergyCalculation(targetConfiguration_);

    auto nAttempts = 0, nAccepted = 0;
    auto totalDelta = 0.0;

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
            auto eCurrent = kernel->totalEnergy(*i);

            // Loop over number of shakes per Atom
            for (auto n = 0; n < nShakesPerAtom; ++n)
            {
                auto moveInitialPos = i->r();

                // Translate Atom randomly according to the stepsize and update its Cell position
                i->translateCoordinates(Vector3::randomUnit() * stepSize);
                targetConfiguration_->updateAtomLocation(i);

                // Calculate new energy
                auto eNew = kernel->totalEnergy(*i);

                // Trial the transformed Atom position
                auto delta = (eNew.totalUnbound() + eNew.geometry() * termScale) -
                             (eCurrent.totalUnbound() + eCurrent.geometry() * termScale);
                auto accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));

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
