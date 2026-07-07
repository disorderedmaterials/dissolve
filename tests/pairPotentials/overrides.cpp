// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/energy.h"
#include "nodes/neutronSQ.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
// Calculate the van der Waals energy of our water test system 3000
TEST(PairPotentialOverridesTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);

    // Set all atomtype charges zero
    auto waterSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterSpeciesNode);
    for (auto &at : waterSpeciesNode->species().atomTypes())
        at->setCharge(0.0);

    // Run the graph from the Import node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Test energy with various states of an override potential
    const auto expectedVanDerWaalsEnergy = 1716.032 + 54.1342;
    auto productionEnergy = kernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
    EXPECT_NEAR(productionEnergy.pairPotential.interMolecular, expectedVanDerWaalsEnergy, 4.3e-2);

    // Set all short-range interaction potentials to zero
    for (auto &at : waterSpeciesNode->species().atomTypes())
        at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::Undefined, "");

    // TODO Need to manually clear the pair potential data so it is correctly regenerated
    testGraph.clearPairPotentials();

    // Get a new kernel - total energy should now be zero
    kernel = testGraph.createEnergyKernel(cfg);
    productionEnergy = kernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
    EXPECT_NEAR(productionEnergy.pairPotential.interMolecular, 0.0, 4.3e-2);

    // Create an override potential to describe the OW-OW interaction (Off to begin with
    auto override = testGraph.addPairPotentialOverride("OW", "OW", PairPotentialOverride::PairPotentialOverrideType::Off,
                                                       {Functions1D::Form::LennardJones126, "epsilon=0.6503 sigma=3.165492"});

    // Get a new kernel - total energy should still be zero
    kernel = testGraph.createEnergyKernel(cfg);
    productionEnergy = kernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
    EXPECT_NEAR(productionEnergy.pairPotential.interMolecular, 0.0, 4.3e-2);

    // Turn the potential to "Add"
    override->setType(PairPotentialOverride::Add);

    // Get a new kernel - total energy should now be as expected
    kernel = testGraph.createEnergyKernel(cfg);
    productionEnergy = kernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
    EXPECT_NEAR(productionEnergy.pairPotential.interMolecular, expectedVanDerWaalsEnergy, 4.3e-2);

    // Turn the potential to "Replace"
    override->setType(PairPotentialOverride::Replace);

    // Get a new kernel - total energy should still be as expected
    kernel = testGraph.createEnergyKernel(cfg);
    productionEnergy = kernel->totalEnergy({Kernel::CalculationFlags::ExcludeGeometric});
    EXPECT_NEAR(productionEnergy.pairPotential.interMolecular, expectedVanDerWaalsEnergy, 4.3e-2);
}

} // namespace UnitTest
