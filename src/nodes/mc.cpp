// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/mc.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "nodes/dissolve.h"
#include "nodes/mcCommon.h"

MCNode::MCNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required});
    addInput("Temperature", "Temperature (K)", temperature_);

    // Options
    addOption("NTrials", "Number of attempts per move", nTrials_);
    addOption("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);

    addOption("RotationStepSizeMin", "Minimum step size for rotations (degrees)", rotationStepSizeMin_);
    addOption("RotationStepSizeMax", "Maximum step size for rotations (degrees)", rotationStepSizeMax_);
    addOption("TranslationStepSizeMin", "Minimum step size for translations (Angstroms)", translationStepSizeMin_);
    addOption("TranslationStepSizeMax", "Maximum step size for translations (Angstroms)", translationStepSizeMax_);

    // Outputs
    addOutput("Configuration", "Output configuration", targetConfiguration_);

    // Serialisables
    addSerialisable("RotationStepSize", rotationStepSize_);
    addSerialisable("TranslationStepSize", translationStepSize_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view MCNode::type() const { return "MC"; }

// Return short summary of the node's purpose
std::string_view MCNode::summary() const { return "Perform standard Monte Carlo evolution on the target configuration"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult MCNode::process()
{
    // Retrieve control parameters from Configuration
    const auto rRT = 1.0 / (.008314472 * temperature_.asDouble());

    // Get options
    auto nTrials = nTrials_.asInteger();

    // Print argument/parameter summary
    message("Performing {} moves(s) per term.\n", nTrials);
    message("Step size for translation adjustments is {:.5f} Angstroms (allowed range is {} <= "
            "delta <= {}).\n",
            translationStepSize_, translationStepSizeMin_.asDouble(), translationStepSizeMax_.asDouble());
    message("Step size for rotation adjustments is {:.5f} degrees (allowed range is {} <= delta <= {}).\n", rotationStepSize_,
            rotationStepSizeMin_.asDouble(), rotationStepSizeMax_.asDouble());
    message("\n");

    // Prepare for energy calculation, generate kernel
    auto kernel = dissolveGraph()->createEnergyKernel(targetConfiguration_);

    auto nRotationAttempts = 0, nTranslationAttempts = 0, nRotationsAccepted = 0, nTranslationsAccepted = 0,
         nGeneralAttempts = 0;
    auto totalDelta = 0.0;
    const auto &box = targetConfiguration_->box();

    /*
     * In order to be able to adjust translation and rotational steps independently, we will perform 80% of moves
     * including both a translation a rotation, 10% using only translations, and 10% using only rotations.
     */

    // Set initial random offset for our counter determining whether to perform R+T, R, or T.
    auto count = int(DissolveMath::random() * 10);
    bool rotate, translate;

    Timer timer;

    // Loop over molecules in the configuration
    for (auto &molecule : targetConfiguration_->molecules())
    {
        // Store the current molecule coordinates
        auto currentCoordinates = molecule->atomCoordinates();

        // Calculate reference pair potential energy for Molecule, excluding all intramolecular contributions
        auto currentEnergy = kernel
                                 ->totalEnergy(*molecule, {Kernel::CalculationFlags::ExcludeGeometric,
                                                           Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential})
                                 .total();

        // Loop over number of shakes per atom
        for (auto trial = 0; trial < nTrials; ++trial)
        {
            // Determine what move(s) will we attempt
            auto rotate = count != 1;
            auto translate = count != 0;

            // Create a random translation vector and apply it to the Molecule's centre
            if (translate)
            {
                molecule->translate(Vector3::randomUnit() * translationStepSize_);

                ++nRotationAttempts;
            }

            // Create a random rotation matrix and apply it to the Molecule
            if (rotate)
            {
                molecule->transform(box, Matrix3::createRotationXY(DissolveMath::randomPlusMinusOne() * rotationStepSize_,
                                                                   DissolveMath::randomPlusMinusOne() * rotationStepSize_));

                ++nTranslationAttempts;
            }

            // Update cell locations for atoms in the molecule
            targetConfiguration_->updateAtomLocations(molecule);

            // Calculate new energy
            auto newEnergy = kernel
                                 ->totalEnergy(*molecule, {Kernel::CalculationFlags::ExcludeGeometric,
                                                           Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential})
                                 .total();

            // Check energy delta
            auto delta = newEnergy - currentEnergy;
            auto accept = delta < 0 ? true : (DissolveMath::random() < exp(-delta * rRT));
            if (accept)
            {
                // Accept new (current) position of target Atoms
                currentCoordinates = molecule->atomCoordinates();
                currentEnergy = newEnergy;

                // Update totals
                totalDelta += delta;
                if (rotate)
                    ++nRotationsAccepted;
                if (translate)
                    ++nTranslationsAccepted;
            }
            else
            {
                // Revert to the previous coordinates
                molecule->setAtomCoordinates(currentCoordinates);
                targetConfiguration_->updateAtomLocations(molecule);
            }

            // Increase and fold move type counter
            if (++count > 9)
                count = 0;
        }
    }

    timer.stop();

    message("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

    // Calculate and print acceptance rates
    auto transRate = double(nTranslationsAccepted) / nTranslationAttempts;
    auto rotRate = double(nRotationsAccepted) / nRotationAttempts;
    message("Total number of attempted moves was {} ({} elapsed)\n", nGeneralAttempts, timer.totalTimeString());
    message("Overall translation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * transRate,
            nTranslationsAccepted, nTranslationAttempts);
    message("Overall rotation acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * rotRate, nRotationsAccepted,
            nRotationAttempts);

    // Update step sizes
    translationStepSize_ = MCCommon::updateStepSize(translationStepSize_, nTranslationAttempts, nTranslationsAccepted,
                                                    targetAcceptanceRate_.asDouble(), translationStepSizeMin_.asDouble(),
                                                    translationStepSizeMax_.asDouble());
    rotationStepSize_ =
        MCCommon::updateStepSize(rotationStepSize_, nRotationAttempts, nRotationsAccepted, targetAcceptanceRate_.asDouble(),
                                 rotationStepSizeMin_.asDouble(), rotationStepSizeMax_.asDouble());
    message("Updated step size for translations is {:.5f} Angstroms.\n", translationStepSize_);
    message("Updated step size for rotations is {:.5f} degrees.\n", rotationStepSize_);

    // Mark the configuration as having been modified
    if ((nRotationsAccepted > 0) || (nTranslationsAccepted > 0))
        targetConfiguration_->notifyAtomicPositionsChanged();

    return NodeConstants::ProcessResult::Success;
}
