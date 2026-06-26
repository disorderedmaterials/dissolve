// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/force.h"
#include "nodes/dissolve.h"
#include "nodes/species.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
std::vector<double> cutoffs = {10.0, 11.425, 11.5, 13.7875, 13.8, 14.999, 15.0};

TEST(PairPotentialCutoffTest, ShortRange)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);

    // Remove charges from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto hw = waterNode->species().findAtomType("HW");
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(hw && ow);
    hw->setCharge(0.0);
    ow->setCharge(0.0);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");

    // Check consistency between production and test forces at different cutoffs
    std::vector<Vector3> pairPotentialForces, geometryForces;
    for (auto cutoff : cutoffs)
    {
        PairPotential::setRange(cutoff, 1.0e-4);
        auto kernel = testGraph.createForceKernel(cfg);
        checkForceConsistency(kernel, pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});
    }
}

TEST(PairPotentialCutoffTest, Coulomb)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);

    // Remove charges from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(ow);
    ow->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=0.0");

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");

    // Check consistency between production and test forces at different cutoffs
    std::vector<Vector3> pairPotentialForces, geometryForces;
    for (auto cutoff : cutoffs)
    {
        PairPotential::setRange(cutoff, 1.0e-4);
        auto kernel = testGraph.createForceKernel(cfg);
        checkForceConsistency(kernel, pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});
    }
}

} // namespace UnitTest