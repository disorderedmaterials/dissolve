// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "kernels/energy.h"
#include "main/dissolve.h"
#include "math/mathFunc.h"
#include "nodes/species.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(CellsTest, Basic)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"Ar", 1}, {"species/water-dlpoly.toml", 267}}, 0.1,
                                              Units::DensityUnits::AtomsPerAngstromUnits));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/solvated_atom/solvated-argon-rcut5.CONFIG"));

    // Set all charge and short-range interaction potentials to zero
    auto arSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Ar"));
    ASSERT_TRUE(arSpeciesNode);
    for (auto &at : arSpeciesNode->species().atomTypes())
    {
        at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::Undefined, "");
        at->setCharge(0.0);
    }
    auto waterSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterSpeciesNode);
    for (auto &at : waterSpeciesNode->species().atomTypes())
    {
        at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::Undefined, "");
        at->setCharge(0.0);
    }

    PairPotential::setRange(9.0);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
    PairPotential::setShortRangeTruncationScheme(PairPotential::NoShortRangeTruncation);

    // Create an override potential to describe the Ar-OW interaction
    testGraph.addPairPotentialOverride("Ar", "OW", PairPotentialOverride::PairPotentialOverrideType::Replace,
                                       {Functions1D::Form::LennardJones126, "epsilon=0.35 sigma=2.166"});

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");

    // Test consistency of energy calculation with DL_POLY reference energies over a range of cutoffs / cell sizes
    std::vector<std::tuple<double, double, double>> states = {
        {9.0, 7.0, -2.0324775}, {9.0, 6.0, -2.0324775}, {9.0, 5.0, -2.0324775}, {9.0, 4.0, -2.0324775}, {9.0, 3.0, -2.0324775},
        {7.0, 7.0, -1.993993},  {7.0, 6.0, -1.993993},  {7.0, 5.0, -1.993993},  {7.0, 4.0, -1.993993},  {5.0, 7.0, -1.904968},
        {5.0, 6.0, -1.904968},  {5.0, 5.0, -1.904968},  {5.0, 4.0, -1.904968}};
    for (auto &state : states)
    {
        auto [rCut, cellSize, refEnergy] = state;

        // Update the cell array in the configuration
        cfg->cells().generate(cfg->box(), cellSize);
        cfg->updateAtomLocations(true);

        // Set pair potential range and initialise an EnergyKernel
        PairPotential::setRange(rCut);
        auto kernel = testGraph.createEnergyKernel(cfg);

        // Calculate production energies (fully optimised)
        auto productionEnergy = kernel->totalEnergy();

        // Calculate total Cell-based energy
        EXPECT_NEAR(refEnergy, productionEnergy.pairPotential.total(), 1.0e-4);

        // Calculate atomic energy from the Ar
        EXPECT_NEAR(refEnergy, kernel->totalEnergy(cfg->atom(0)).total(), 1.0e-4);
    }
}
} // namespace UnitTest
