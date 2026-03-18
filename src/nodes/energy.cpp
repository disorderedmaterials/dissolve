// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/energy.h"
#include "base/sysFunc.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "nodes/dissolve.h"

EnergyNode::EnergyNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Configuration to calculate energy for", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOutput<Configuration *>("Configuration", "Output configuration", targetConfiguration_);

    // Options
    addOption<double>("StabilityThreshold",
                      "Threshold value at which energy is deemed stable over the defined windowing period",
                      stabilityThreshold_);
    addOption<int>("StabilityWindow", "Number of points over which to assess the stability of the energy (per Configuration)",
                   stabilityWindow_);
}

/*
 * Node
 */

std::string_view EnergyNode::type() const { return "Energy"; }

std::string_view EnergyNode::summary() const { return "Calculate total energy of a configuration"; }

// Run main processing
NodeConstants::ProcessResult EnergyNode::process()
{
    message("\n");

    message("Calculating total energy for Configuration '{}'...\n", targetConfiguration_->name());

    auto kernel = dissolveGraph()->createEnergyKernel(targetConfiguration_);

    Timer timer;
    auto energy = kernel->totalEnergy();
    timer.stop();

    message("Time to do energy was {}.\n", timer.totalTimeString());
    message("Total energy is {:15.9e} kJ/mol ({:15.9e} kJ/mol pair potential + {:15.9e} kJ/mol "
            "geometry ({:15.9e} bond + {:15.9e} angle + {:15.9e} torsion + {:15.9e} improper).\n",
            energy.pairPotential.total() + energy.geometry.total(), energy.pairPotential.total(), energy.geometry.total(),
            energy.geometry.bondEnergy, energy.geometry.angleEnergy, energy.geometry.torsionEnergy,
            energy.geometry.improperEnergy);

    // Update histories
    totalEnergyHistory_.push(energy.pairPotential.total() + energy.geometry.total(), podHistoryLength_);
    totalPairPotentialHistory_.push(energy.pairPotential.total(), podHistoryLength_);
    totalMoleculePPHistory_.push(energy.pairPotential.intraMolecular, podHistoryLength_);
    totalCohesiveHistory.push(energy.pairPotential.interMolecular, podHistoryLength_);
    totalGeometryHistory_.push(energy.geometry.total(), podHistoryLength_);
    bondHistory_.push(energy.geometry.bondEnergy, podHistoryLength_);
    angleHistory_.push(energy.geometry.angleEnergy, podHistoryLength_);
    torsionHistory_.push(energy.geometry.torsionEnergy, podHistoryLength_);
    improperHistory_.push(energy.geometry.improperEnergy, podHistoryLength_);

    // Determine stability of energy
    // Check number of points already stored for the Configuration
    auto grad = 0.0;
    auto stable = false;
    if (stabilityWindow_ > totalEnergyHistory_.history().size())
        message("Too few points to assess stability.\n");
    else
    {
        auto yMean = 0.0;
        grad = Regression::linearGradient(totalEnergyHistory_.history(), stabilityWindow_, yMean);
        auto thresholdValue = fabs(stabilityThreshold_ * yMean);
        stable = fabs(grad) < thresholdValue;

        message("Gradient of last {} points is {:e} kJ/mol/step (absolute threshold value is "
                "{:e}, stable = {}).\n",
                stabilityWindow_, grad, thresholdValue, DissolveSys::btoa(stable));
    }

    targetConfiguration_->setEnergyGradient(grad);
    targetConfiguration_->setEnergyStable(stable);

    return NodeConstants::ProcessResult::Success;
}
