// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/energy.h"
#include "kernels/force.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(Water1000EnergyTest, Full)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = testEnergyConsistency(kernel);

    // Interatomic energy: 1716.032 LJ + 54.1342 correction + -29163.384451743802 Coulomb
    EXPECT_NEAR(1716.032 + 54.1342 - 29163.384451743802, productionEnergy.pairPotential.interMolecular, 4.3e-2);

    // Intramolecular energy: 4.664830886619E+03 bond + 2.528107096550E+03 angle
    EXPECT_NEAR(4.664830886619E+03 + 2.528107096550E+03, productionEnergy.geometry.total(), 3.0e-2);
    EXPECT_NEAR(4.664830886619E+03, productionEnergy.geometry.bondEnergy, 3.0e-2);
    EXPECT_NEAR(2.528107096550E+03, productionEnergy.geometry.angleEnergy, 7.0e-5);
}

TEST(Water1000ForceTest, Full)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/full.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    testForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    testReferenceForceConsistency(pairPotentialForces, geometryForces,
                                  importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.9);
}

TEST(Water1000ForceTest, Intra)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/intra.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    testForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    std::vector<Vector3> noPP(geometryForces.size());
    testReferenceForceConsistency(noPP, geometryForces, importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.9);
}

TEST(Water1000EnergyTest, ShortRangeOnly)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
    PairPotential::setRange(15.0, 1.0e-4);

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

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = testEnergyConsistency(kernel);

    // Interatomic energy: 1716.032 LJ + 54.1342 correction
    EXPECT_NEAR(1716.032 + 54.1342, productionEnergy.pairPotential.interMolecular, 4.3e-2);
}

TEST(Water1000ForceTest, ShortRangeOnly)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/vdw.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
    PairPotential::setRange(15.0, 1.0e-4);

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

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    testForceConsistency(kernel, pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});

    // Check agreement with external reference forces
    testReferenceForceConsistency(pairPotentialForces, geometryForces,
                                  importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.6e-1);
}

TEST(Water1000EnergyTest, ShiftedCoulombOnly)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Remove short range from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(ow);
    ow->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=3.0");

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = testEnergyConsistency(kernel);

    // Interatomic energy: -29163.384451743802 Coulomb
    EXPECT_NEAR(-29163.384451743802, productionEnergy.pairPotential.interMolecular, 4.3e-2);
}

TEST(Water1000ForceTest, CoulombOnly)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/coulomb.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Remove short range from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(ow);
    ow->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=0.0");

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    testForceConsistency(kernel, pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});

    // Check agreement with external reference forces
    testReferenceForceConsistency(pairPotentialForces, geometryForces,
                                  importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.6e-1);
}

TEST(Water1000ForceTest, ShiftedCoulombOnly)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/shifted.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setRange(15.0, 1.0e-4);

    // Remove charges from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto hw = waterNode->species().findAtomType("HW");
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(hw && ow);
    ow->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=0.0");

    // Run the graph from the head node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    testForceConsistency(kernel, pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});

    // Check agreement with external reference forces
    testReferenceForceConsistency(pairPotentialForces, geometryForces,
                                  importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.6e-1);
}
TEST(Water1000EnergyTest, Override)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/CONFIG"));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Set all charge and short-range interaction potentials to zero
    auto waterSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterSpeciesNode);
    for (auto &at : waterSpeciesNode->species().atomTypes())
    {
        at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::Undefined, "");
        at->setCharge(0.0);
    }

    // Create an override potential to describe the OW-OW interaction
    testGraph.addPairPotentialOverride("OW", "OW", PairPotentialOverride::PairPotentialOverrideType::Replace,
                                       {Functions1D::Form::LennardJones126, "epsilon=0.6503 sigma=3.165492"});

    // Run the graph from the Import node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = kernel->totalEnergy();

    // Interatomic energy: 1716.032 LJ + 54.1342 correction
    EXPECT_NEAR(1716.032 + 54.1342, productionEnergy.pairPotential.interMolecular, 4.3e-2);
}

TEST(Water1000ForceTest, Overrides)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportDLPOLYStructure", "dlpoly/water1000/vdw.REVCON"));
    auto importNode = testGraph.findNode("ImportDLPOLYStructure");
    ASSERT_TRUE(importNode);

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Set all charge and short-range interaction potentials to zero
    auto waterSpeciesNode = dynamic_cast<SpeciesNode *>(testGraph.findNode("Water"));
    ASSERT_TRUE(waterSpeciesNode);
    for (auto &at : waterSpeciesNode->species().atomTypes())
    {
        at->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::Undefined, "");
        at->setCharge(0.0);
    }

    // Create an override potential to describe the OW-OW interaction
    testGraph.addPairPotentialOverride("OW", "OW", PairPotentialOverride::PairPotentialOverrideType::Replace,
                                       {Functions1D::Form::LennardJones126, "epsilon=0.6503 sigma=3.165492"});

    // Run the graph from the Import node to set up the configuration
    ASSERT_EQ(testGraph.fetchHead()->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(testGraph.fetchHead()->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = testGraph.fetchHead()->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    kernel->totalForces(pairPotentialForces, geometryForces, {Kernel::CalculationFlags::ExcludeGeometric});

    // Check agreement with external reference forces
    testReferenceForceConsistency(pairPotentialForces, geometryForces,
                                  importNode->getOutputValue<std::vector<Vector3>>("Forces"), 1.6e-1);
}
} // namespace UnitTest
