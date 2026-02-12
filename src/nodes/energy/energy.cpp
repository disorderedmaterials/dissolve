// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/energy/energy.h"
#include "base/sysFunc.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include "math/regression.h"
#include "nodes/dissolve.h"

EnergyNode::EnergyNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
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

    auto kernel = dissolveGraph()->createEnergyCalculation(targetConfiguration_);

    Timer timer;
    auto &&[ppEnergy, geometryEnergy] = calculateEnergy(targetConfiguration_, kernel);
    timer.stop();

    message("Time to do energy was {}.\n", timer.totalTimeString());
    message("Total energy is {:15.9e} kJ/mol ({:15.9e} kJ/mol pair potential + {:15.9e} kJ/mol "
            "geometry ({:15.9e} bond + {:15.9e} angle + {:15.9e} torsion + {:15.9e} improper).\n",
            ppEnergy.total() + geometryEnergy.total(), ppEnergy.total(), geometryEnergy.total(), geometryEnergy.bondEnergy,
            geometryEnergy.angleEnergy, geometryEnergy.torsionEnergy, geometryEnergy.improperEnergy);

    // Update histories
    // TODO
    Data1D totalEnergyArray;
    totalEnergyArray.addPoint(dissolve().iteration(), ppEnergy.total() + geometryEnergy.total());

    // Determine stability of energy
    // Check number of points already stored for the Configuration
    auto grad = 0.0;
    auto stable = false;
    if (stabilityWindow_ > totalEnergyArray.nValues())
        message("Too few points to assess stability.\n");
    else
    {
        auto yMean = 0.0;
        grad = Regression::linear(totalEnergyArray, stabilityWindow_, yMean);
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

/*
 * Functions
 */

// Calculate energy components
std::pair<PairPotentialEnergyValue, GeometryEnergyValue>
EnergyNode::calculateEnergy(const Configuration *cfg, const std::unique_ptr<EnergyKernel> &kernel,
                            PairPotentialEnergyType ppType, bool calculateGeometryEnergy)
{
    // Calculate pair potential energy
    PairPotentialEnergyValue ppEnergy;
    if (ppType != PairPotentialEnergyType::None)
        ppEnergy = kernel->totalPairPotentialEnergy(ppType == PairPotentialEnergyType::Full);

    const auto &molecules = cfg->molecules();
    auto unaryOp = [&](const auto &mol) -> GeometryEnergyValue
    {
        GeometryEnergyValue localEnergies{.bondEnergy = 0.0, .angleEnergy = 0.0, .torsionEnergy = 0.0, .improperEnergy = 0.0};

        // Loop over Bond
        localEnergies.bondEnergy +=
            std::accumulate(mol->species()->bonds().cbegin(), mol->species()->bonds().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &t)
                            { return acc + kernel->bondEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ())); });

        // Loop over Angle
        localEnergies.angleEnergy += std::accumulate(
            mol->species()->angles().cbegin(), mol->species()->angles().cend(), 0.0,
            [&mol, &kernel](auto const acc, const auto &t)
            { return acc + kernel->angleEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ()), *mol->atom(t.indexK())); });

        // Loop over Torsions
        localEnergies.torsionEnergy +=
            std::accumulate(mol->species()->torsions().cbegin(), mol->species()->torsions().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &t)
                            {
                                return acc + kernel->torsionEnergy(t, *mol->atom(t.indexI()), *mol->atom(t.indexJ()),
                                                                   *mol->atom(t.indexK()), *mol->atom(t.indexL()));
                            });

        localEnergies.improperEnergy +=
            std::accumulate(mol->species()->impropers().cbegin(), mol->species()->impropers().cend(), 0.0,
                            [&mol, &kernel](auto const acc, const auto &imp)
                            {
                                return acc + kernel->improperEnergy(imp, *mol->atom(imp.indexI()), *mol->atom(imp.indexJ()),
                                                                    *mol->atom(imp.indexK()), *mol->atom(imp.indexL()));
                            });

        return localEnergies;
    };

    GeometryEnergyValue geometryEnergy;
    if (calculateGeometryEnergy)
        geometryEnergy = dissolve::transform_reduce(ParallelPolicies::par, molecules.begin(), molecules.end(),
                                                    GeometryEnergyValue(), std::plus<>(), unaryOp);

    return {ppEnergy, geometryEnergy};
}
