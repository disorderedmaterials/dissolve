// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "nodes/energy.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(WaterSPCFwEnergyTest, Full)
{
    GraphTestData data;
    createWaterGraph(
        &data.graphRoot, 1000,
        CoordinateImportFileFormat("dlpoly/water1000/CONFIG", CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Run the graph from the Import node to set up the configuration
    auto importNode = data.graphRoot.findNode("Import");
    ASSERT_TRUE(importNode);
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");
    auto kernel = data.graphRoot.dissolveGraph()->createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto &&[productionPP, productionGeometry] = checkEnergyConsistency(cfg, kernel);

    // Interatomic energy: 1716.032 LJ + 54.1342 correction + -29163.384451743802 Coulomb
    EXPECT_NEAR(1716.032 + 54.1342 - 29163.384451743802, productionPP.interMolecular(), 4.3e-2);

    // Intramolecular energy: 4.664830886619E+03 bond + 2.528107096550E+03 angle
    EXPECT_NEAR(4.664830886619E+03 + 2.528107096550E+03, productionGeometry.total(), 3.0e-2);
    EXPECT_NEAR(4.664830886619E+03, productionGeometry.bondEnergy, 3.0e-2);
    EXPECT_NEAR(2.528107096550E+03, productionGeometry.angleEnergy, 7.0e-5);
}

TEST(WaterSPCFwEnergyTest, ShortRangeOnly)
{
    GraphTestData data;
    createWaterGraph(
        &data.graphRoot, 1000,
        CoordinateImportFileFormat("dlpoly/water1000/CONFIG", CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
    PairPotential::setRange(15.0, 1.0e-4);

    // Remove charges from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(data.graphRoot.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto hw = waterNode->species().findAtomType("HW");
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(hw && ow);
    hw->setCharge(0.0);
    ow->setCharge(0.0);

    // Run the graph from the Import node to set up the configuration
    auto importNode = data.graphRoot.findNode("Import");
    ASSERT_TRUE(importNode);
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");
    auto kernel = data.graphRoot.dissolveGraph()->createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto &&[productionPP, productionGeometry] = checkEnergyConsistency(cfg, kernel);

    // Interatomic energy: 1716.032 LJ + 54.1342 correction
    EXPECT_NEAR(1716.032 + 54.1342, productionPP.interMolecular(), 4.3e-2);
}

TEST(WaterSPCFwEnergyTest, ElectrostaticsOnly)
{
    GraphTestData data;
    createWaterGraph(
        &data.graphRoot, 1000,
        CoordinateImportFileFormat("dlpoly/water1000/CONFIG", CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setRange(15.0, 1.0e-4);

    // Remove charges from atom types
    auto waterNode = dynamic_cast<SpeciesNode *>(data.graphRoot.findNode("Water"));
    ASSERT_TRUE(waterNode);
    auto ow = waterNode->species().findAtomType("OW");
    ASSERT_TRUE(ow);
    ow->interactionPotential().setFormAndParameters(ShortRangeFunctions::Form::LennardJones, "epsilon=0.0 sigma=3.0");

    // Run the graph from the Import node to set up the configuration
    auto importNode = data.graphRoot.findNode("Import");
    ASSERT_TRUE(importNode);
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");
    auto kernel = data.graphRoot.dissolveGraph()->createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto &&[productionPP, productionGeometry] = checkEnergyConsistency(cfg, kernel);

    // Interatomic energy: -29163.384451743802 Coulomb
    EXPECT_NEAR(-29163.384451743802, productionPP.interMolecular(), 4.3e-2);
}

} // namespace UnitTest