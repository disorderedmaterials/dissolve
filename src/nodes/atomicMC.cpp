// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/atomicMC.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "math/mc.h"
#include "nodes/dissolve.h"

AtomicMCNode::AtomicMCNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the node", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});
    addInput("Temperature", "Temperature (K)", temperature_);

    // Options
    addOption("ShakesPerAtom", "Number of shakes to attempt per atom", nShakesPerAtom_);
    addOption("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);
    addOption("StepSizeMax", "Maximum allowed value for step size, in Angstroms", stepSizeMax_);
    addOption("StepSizeMin", "Minimum allowed value for step size, in Angstroms", stepSizeMin_);

    // Outputs
    addOutput("Configuration", "Output configuration", targetConfiguration_);

    // Serialisables
    addSerialisable("stepSize", stepSize_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view AtomicMCNode::type() const { return "AtomicMC"; }

// Return short summary of the node's purpose
std::string_view AtomicMCNode::summary() const { return "Perform atomic Monte Carlo on the target configuration"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult AtomicMCNode::process()
{
    // Get options
    auto nShakesPerAtom = nShakesPerAtom_.asInteger();

    // Retrieve control parameters from Configuration
    const auto rRT = 1.0 / (.008314472 * temperature_.asDouble());

    // Print argument/parameter summary
    message("Performing {} shake(s) per Atom\n", nShakesPerAtom);
    message("Step size for adjustments is {:.5f} Angstroms (allowed range is {} <= delta <= {}).\n", stepSize_,
            stepSizeMin_.asDouble(), stepSizeMax_.asDouble());
    message("Target acceptance rate is {}.\n", targetAcceptanceRate_.asDouble());
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
                *i += Vector3::randomUnit() * stepSize_;
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
    message("Total number of attempted moves was {} ({} elapsed)\n", nAttempts, timer.totalTimeString());

    message("Overall acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rate, nAccepted, nAttempts);

    // Update step size
    stepSize_ = MonteCarloCommon::updateStepSize(stepSize_, nAttempts, nAccepted, targetAcceptanceRate_.asDouble(),
                                                 stepSizeMin_.asDouble(), stepSizeMax_.asDouble());
    message("Updated step size is {} Angstroms.\n", stepSize_);

    // Mark the configuration as having been modified
    if (nAccepted > 0)
        targetConfiguration_->notifyAtomicPositionsChanged();

    return NodeConstants::ProcessResult::Success;
}
