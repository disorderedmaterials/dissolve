// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/atomicMC.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "nodes/dissolve.h"

AtomicMCNode::AtomicMCNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the node", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});
    addInput<Number>("Temperature", "Temperature (K)", temperature_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Options
    addOption<Number>("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addOption<Number>("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);
    addOption<Number>("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addOption<Number>("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", targetConfiguration_);

    // Serialisables
    addSerialisable("stepSize", stepSize_);
}

/*
 * Definition
 */

std::string_view AtomicMCNode::type() const { return "AtomicMC"; }

std::string_view AtomicMCNode::summary() const
{
    return "Perform a Monte Carlo trial move on every atom in the target configuration";
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AtomicMCNode::process()
{
    // Get numeric input data
    auto nShakesPerAtom = nShakesPerAtom_.asInteger();
    auto stepSize = stepSize_.asDouble();
    auto stepSizeMax = stepSizeMax_.asDouble();
    auto stepSizeMin = stepSizeMin_.asDouble();
    auto targetAcceptanceRate = targetAcceptanceRate_.asDouble();

    // Retrieve control parameters from Configuration
    const auto rRT = 1.0 / (.008314472 * temperature_.asDouble());

    // Print argument/parameter summary
    message("Performing {} shake(s) per Atom\n", nShakesPerAtom);
    message("Step size for adjustments is {:.5f} Angstroms (allowed range is {} <= delta <= {}).\n", stepSize, stepSizeMin,
            stepSizeMax);
    message("Target acceptance rate is {}.\n", targetAcceptanceRate);
    message("\n");

    // Prepare for energy calculation, generate kernel
    auto kernel = dissolveGraph()->createEnergyKernel(targetConfiguration_);

    auto nAttempts = 0, nAccepted = 0;
    auto totalDelta = 0.0;

    Timer timer;

    // Loop over Molecules
    for (auto mol : targetConfiguration_->molecules())
    {
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
                *i += Vector3::randomUnit() * stepSize;
                targetConfiguration_->updateAtomLocation(i);

                // Calculate new energy
                auto eNew = kernel->totalEnergy(*i);

                // Trial the transformed Atom position
                auto delta =
                    (eNew.totalUnbound() + eNew.geometry.total()) - (eCurrent.totalUnbound() + eCurrent.geometry.total());
                auto accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));

                if (accept)
                {
                    // Store incremental total energy and new reference energy
                    totalDelta += delta;
                    eCurrent = eNew;

                    // Increase attempt counter
                    ++nAccepted;
                }
                else
                {
                    // Move not accepted - revert to initial position
                    i->setR(moveInitialPos);
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
