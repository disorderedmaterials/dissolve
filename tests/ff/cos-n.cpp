// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "data/ff/library.h"
#include "tests/graphData.h"
#include "tests/tempFile.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(CosNTorsionEnergyTest, POE)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/poe_torsions_only.toml", 64}}, {22.98582, 22.98582, 22.98582}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/poe64_torsions/torsions-final.str"));

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Intramolecular energy: 183.4801   # (2.866876 per molecule) * 64 molecules
    EXPECT_NEAR(183.4801, productionEnergy.geometry.torsionEnergy, 1.0e-2);
}

TEST(CosNTorsionForcesTest, POE)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/poe_torsions_only.toml", 64}}, {22.98582, 22.98582, 22.98582}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/poe64_torsions/torsions-final.str"));
    auto importNode = testGraph.findNode("ImportMoscitoStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   importNode->getOutputValue<std::vector<Vector3>>("Forces"), 6.2e-2);
}

TEST(CosNTorsionEnergyTest, Py4OHNTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py4oh_torsions_only.toml", 1}, {"species/ntf2_torsions_only.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py4oh_torsions/py4oh-ntf2-final.str"));

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Intramolecular energy: 51.050222   # (25.525111 per molecule) * 2 molecules
    EXPECT_NEAR(51.050222, productionEnergy.geometry.torsionEnergy, 2.0e-5);
}

TEST(CosNTorsionForcesTest, Py4OHNTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py4oh_torsions_only.toml", 1}, {"species/ntf2_torsions_only.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py4oh_torsions/py4oh-ntf2-final.str"));
    auto importNode = testGraph.findNode("ImportMoscitoStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   importNode->getOutputValue<std::vector<Vector3>>("Forces"), 9.0e-3);
}

TEST(CosNImproperEnergyTest, Py4OHNTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py4oh_impropers_only.toml", 1}, {"species/ntf2_no_terms.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py4oh_impropers/py4oh-ntf2-final.str"));

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Intramolecular energy: 0.055228   # (0.027614 per molecule) * 2 molecules
    EXPECT_NEAR(0.055228, productionEnergy.geometry.improperEnergy, 1.0e-6);
}

TEST(CosNImproperForcesTest, Py4OHNTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py4oh_impropers_only.toml", 1}, {"species/ntf2_no_terms.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py4oh_impropers/py4oh-ntf2-final.str"));
    auto importNode = testGraph.findNode("ImportMoscitoStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   importNode->getOutputValue<std::vector<Vector3>>("Forces"), 7.0e-2);
}

TEST(CosNTorsionEnergyTest, Py5NTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py5_torsions_only.toml", 1}, {"species/ntf2_torsions_only.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py5_torsions/py5-ntf2-final.str"));

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Intramolecular energy: 39.29711  # (19.648555 per molecule) * 2 molecules
    EXPECT_NEAR(39.29711, productionEnergy.geometry.torsionEnergy, 5.0e-5);
}

TEST(CosNTorsionForcesTest, Py5NTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py5_torsions_only.toml", 1}, {"species/ntf2_torsions_only.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py5_torsions/py5-ntf2-final.str"));
    auto importNode = testGraph.findNode("ImportMoscitoStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   importNode->getOutputValue<std::vector<Vector3>>("Forces"), 9.0e-2);
}

TEST(CosNImproperEnergyTest, Py5NTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py5_impropers_only.toml", 1}, {"species/ntf2_no_terms.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py5_impropers/py5-ntf2-final.str"));

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Intramolecular energy: 0.34961  # (0.174805 per molecule) * 2 molecules
    EXPECT_NEAR(0.34961, productionEnergy.geometry.improperEnergy, 2.0e-6);
}

TEST(CosNImproperForcesTest, Py5NTf2)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration(
        "Box", {{"species/py5_impropers_only.toml", 1}, {"species/ntf2_no_terms.toml", 1}}, {20.0, 20.0, 20.0}));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportMoscitoStructure", "moscito/py5_impropers/py5-ntf2-final.str"));
    auto importNode = testGraph.findNode("ImportMoscitoStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(9.0, 5.0e-3);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   importNode->getOutputValue<std::vector<Vector3>>("Forces"), 4.0e-3);
}

} // namespace UnitTest
