// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/intraMC.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "math/mathFunc.h"
#include "math/mc.h"
#include "nodes/dissolve.h"

IntraMCNode::IntraMCNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required});
    addInput("Temperature", "Temperature (K)", temperature_)->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Options
    addOption("NTrials", "Number of attempts per move", nTrials_);
    addOption("TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", targetAcceptanceRate_);

    addOption("BondStepSizeMin", "Minimum step size for bond adjustments (Angstroms)", bondStepSizeMin_);
    addOption("BondStepSizeMax", "Maximum step size for bond adjustments (Angstroms)", bondStepSizeMax_);
    addOption("AngleStepSizeMin", "Minimum step size for angle rotations (degrees)", angleStepSizeMin_);
    addOption("AngleStepSizeMax", "Maximum step size for angle rotations (degrees)", angleStepSizeMax_);
    addOption("TorsionStepSizeMin", "Minimum step size for torsion twists (degrees)", torsionStepSizeMin_);
    addOption("TorsionStepSizeMax", "Maximum step size for torsion twists (degrees)", torsionStepSizeMax_);

    // Outputs
    addOutput("Configuration", "Output configuration", targetConfiguration_);

    // Serialisables
    addSerialisable("BondStepSize", bondStepSize_);
    addSerialisable("AngleStepSize", angleStepSize_);
    addSerialisable("TorsionStepSize", torsionStepSize_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view IntraMCNode::type() const { return "IntraMC"; }

// Return short summary of the node's purpose
std::string_view IntraMCNode::summary() const { return "Perform Monte Carlo evolution on intramolecular terms"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult IntraMCNode::process()
{
    // Retrieve control parameters from Configuration
    const auto rRT = 1.0 / (.008314472 * temperature_.asDouble());

    // Get options
    auto nTrials = nTrials_.asInteger();
    auto targetAcceptanceRate = targetAcceptanceRate_.asDouble();
    auto nBondMovesAccepted = 0, nBondMovesAttempted = 0;
    auto nAngleMovesAccepted = 0, nAngleMovesAttempted = 0;
    auto nTorsionMovesAccepted = 0, nTorsionMovesAttempted = 0;

    // Print argument/parameter summary
    message("Performing {} moves(s) per term.\n", nTrials);
    message("Step size for bond adjustments is {:.5f} Angstroms (allowed range is {} <= "
            "delta <= {}).\n",
            bondStepSize_, bondStepSizeMin_.asDouble(), bondStepSizeMax_.asDouble());
    message("Step size for angle adjustments is {:.5f} degrees (allowed range is {} <= delta <= {}).\n", angleStepSize_,
            angleStepSizeMin_.asDouble(), angleStepSizeMax_.asDouble());
    message("Step size for torsion adjustments is {:.5f} degrees (allowed range is {} <= delta <= {}).\n", torsionStepSize_,
            torsionStepSizeMin_.asDouble(), torsionStepSizeMax_.asDouble());
    message("\n");

    // Prepare for energy calculation, generate kernel
    auto kernel = dissolveGraph()->createEnergyKernel(targetConfiguration_);

    auto totalDelta = 0.0;
    const auto &box = targetConfiguration_->box();

    Timer timer;

    // Loop over molecules in the configuration
    for (auto &molecule : targetConfiguration_->molecules())
    {
        // Store the current molecule coordinates
        auto currentCoordinates = molecule->atomCoordinates();

        // Calculate reference pair potential energy for Molecule
        auto currentEnergy = kernel->totalEnergy(*molecule);

        // Bonds
        if (adjustBonds_)
            for (const auto &bond : molecule->species()->bonds())
            {
                // Get Atom pointers
                auto i = molecule->atom(bond.i()->index());
                auto j = molecule->atom(bond.j()->index());

                // Select random terminus
                auto terminus = DissolveMath::random() > 0.5 ? 1 : 0;

                // Loop over number of trials per term
                for (auto trial = 0; trial < nTrials; ++trial)
                {
                    // Get translation vector, normalise, and apply random delta
                    auto vji = box.minimumVector(i->r(), j->r());
                    vji.normalise();
                    vji *= DissolveMath::randomPlusMinusOne() * bondStepSize_;

                    // Adjust the atoms attached to the selected terminus
                    molecule->translate(vji, bond.attachedAtoms(terminus));

                    // Update Cell positions of the adjusted Atoms
                    targetConfiguration_->updateAtomLocations(molecule, bond.attachedAtoms(terminus));

                    // Calculate new energy and delta
                    auto newEnergy = kernel->totalEnergy(*molecule);
                    auto delta = newEnergy.total() - currentEnergy.total();
                    ++nBondMovesAttempted;

                    // Accept new (current) positions?
                    if (delta < 0 || (DissolveMath::random() < exp(-delta * rRT)))
                    {
                        currentCoordinates = molecule->atomCoordinates();
                        currentEnergy = newEnergy;
                        totalDelta += delta;
                        ++nBondMovesAccepted;
                    }
                    else
                    {
                        // Revert to the previous coordinates
                        molecule->setAtomCoordinates(currentCoordinates);
                        targetConfiguration_->updateAtomLocations(molecule, bond.attachedAtoms(terminus));
                    }
                }
            }

        // Angles
        if (adjustAngles_)
            for (const auto &angle : molecule->species()->angles())
            {
                // Get Atom pointers
                auto i = molecule->atom(angle.i()->index());
                auto j = molecule->atom(angle.j()->index());
                auto k = molecule->atom(angle.k()->index());

                // Select random terminus
                auto terminus = DissolveMath::random() > 0.5 ? 1 : 0;

                // Loop over number of shakes per term
                for (auto trial = 0; trial < nTrials; ++trial)
                {
                    // Get bond vectors and calculate cross product to get rotation axis
                    auto vji = box.minimumVector(j->r(), i->r());
                    auto vjk = box.minimumVector(j->r(), k->r());
                    auto v = vji * vjk;

                    // Transform the Atoms attached to the selected terminus
                    molecule->transform(
                        box, Matrix3::createRotationAxis(v, DissolveMath::randomPlusMinusOne() * angleStepSize_, true), j->r(),
                        angle.attachedAtoms(terminus));

                    // Update Cell positions of the adjusted Atoms
                    targetConfiguration_->updateAtomLocations(molecule, angle.attachedAtoms(terminus));

                    // Calculate new energy and delta
                    auto newEnergy = kernel->totalEnergy(*molecule);
                    auto delta = newEnergy.total() - currentEnergy.total();
                    ++nAngleMovesAttempted;

                    // Accept new (current) positions of the Molecule's Atoms?
                    if (delta < 0 || (DissolveMath::random() < exp(-delta * rRT)))
                    {
                        currentCoordinates = molecule->atomCoordinates();
                        currentEnergy = newEnergy;
                        totalDelta += delta;
                        ++nAngleMovesAccepted;
                    }
                    else
                    {
                        // Revert to the previous coordinates
                        molecule->setAtomCoordinates(currentCoordinates);
                        targetConfiguration_->updateAtomLocations(molecule, angle.attachedAtoms(terminus));
                    }
                }
            }

        // Torsions
        if (adjustTorsions_)
            for (const auto &torsion : molecule->species()->torsions())
            {
                // Refuse to change a torsion which is in a cycle
                if (torsion.inCycle())
                    continue;

                // Get Atom pointers
                auto j = molecule->atom(torsion.j()->index());
                auto k = molecule->atom(torsion.k()->index());

                // Select random terminus
                auto terminus = DissolveMath::random() > 0.5 ? 1 : 0;

                // Loop over number of shakes per term
                for (auto trial = 0; trial < nTrials; ++trial)
                {
                    // Get bond vectors j-k to get rotation axis
                    auto vjk = box.minimumVector(j->r(), k->r());

                    // Transform the Atoms attached to the selected terminus
                    molecule->transform(
                        box, Matrix3::createRotationAxis(vjk, DissolveMath::randomPlusMinusOne() * torsionStepSize_, true),
                        terminus == 0 ? j->r() : k->r(), torsion.attachedAtoms(terminus));

                    // Update Cell positions of the adjusted Atoms
                    targetConfiguration_->updateAtomLocations(molecule, torsion.attachedAtoms(terminus));

                    // Calculate new energy and delta
                    auto newEnergy = kernel->totalEnergy(*molecule);
                    auto delta = newEnergy.total() - currentEnergy.total();
                    ++nTorsionMovesAttempted;

                    // Accept new (current) positions of the Molecule's Atoms?
                    if (delta < 0 || (DissolveMath::random() < exp(-delta * rRT)))
                    {
                        currentCoordinates = molecule->atomCoordinates();
                        currentEnergy = newEnergy;
                        totalDelta += delta;
                        ++nTorsionMovesAccepted;
                    }
                    else
                    {
                        // Revert to the previous coordinates
                        molecule->setAtomCoordinates(currentCoordinates);
                        targetConfiguration_->updateAtomLocations(molecule, torsion.attachedAtoms(terminus));
                    }
                }
            }
    }

    timer.stop();

    message("Total energy delta was {:10.4e} kJ/mol.\n", totalDelta);

    // Calculate and print acceptance rates
    if (adjustBonds_)
    {
        auto bondRate = double(nBondMovesAccepted) / nBondMovesAttempted;
        message("Overall bond acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * bondRate, nBondMovesAccepted,
                nBondMovesAttempted);
        bondStepSize_ =
            MonteCarloCommon::updateStepSize(bondStepSize_, nBondMovesAttempted, nBondMovesAccepted, targetAcceptanceRate,
                                             bondStepSizeMin_.asDouble(), bondStepSizeMax_.asDouble());
        message("Updated step size for bonds is {:.5f} Angstroms.\n", bondStepSize_);
    }
    if (adjustAngles_)
    {
        auto angleRate = double(nAngleMovesAccepted) / nAngleMovesAttempted;
        message("Overall angle acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * angleRate,
                nAngleMovesAccepted, nAngleMovesAttempted);
        angleStepSize_ =
            MonteCarloCommon::updateStepSize(angleStepSize_, nAngleMovesAttempted, nAngleMovesAccepted, targetAcceptanceRate,
                                             angleStepSizeMin_.asDouble(), angleStepSizeMax_.asDouble());
        message("Updated step size for angles is {:.5f} Angstroms.\n", angleStepSize_);
    }
    if (adjustTorsions_)
    {
        auto torsionRate = double(nTorsionMovesAccepted) / nTorsionMovesAttempted;
        message("Overall torsion acceptance rate was {:4.2f}% ({} of {} attempted moves)\n", 100.0 * torsionRate,
                nTorsionMovesAccepted, nTorsionMovesAttempted);
        torsionStepSize_ = MonteCarloCommon::updateStepSize(torsionStepSize_, nTorsionMovesAttempted, nTorsionMovesAccepted,
                                                            targetAcceptanceRate, torsionStepSizeMin_.asDouble(),
                                                            torsionStepSizeMax_.asDouble());
        message("Updated step size for torsions is {:.5f} Angstroms.\n", torsionStepSize_);
    }

    // Mark the configuration as having been modified
    if ((nBondMovesAccepted > 0) || (nAngleMovesAccepted > 0) || (nTorsionMovesAccepted > 0))
        targetConfiguration_->notifyAtomicPositionsChanged();

    return NodeConstants::ProcessResult::Success;
}
