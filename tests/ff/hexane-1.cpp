// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/force.h"
#include "nodes/dissolve.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(Hexane1EnergyTest, Full)
{
    TestGraph testGraph;
    auto lastNode = testGraph.createConfiguration("Box",
                                                  {
                                                      {createHexane, 1},
                                                  },
                                                  0.001);
    auto importNode = testGraph.appendImportCoordinates(
        lastNode,
        CoordinateImportFileFormat("dlpoly/hexane1/CONFIG", CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));
    ASSERT_TRUE(importNode);

    // Set cell dimensions
    auto setCellNode = testGraph.findNode("SetCell");
    ASSERT_TRUE(setCellNode->setOption<Vector3>("Lengths", {30.769064857500, 46.153597286200, 30.769064857500}));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::SpeciesAtoms);
    PairPotential::setRange(12.0, 1.0e-4);

    // Run the graph from the Import node to set up the configuration
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration and create an energy kernel
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createEnergyKernel(cfg);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Interatomic energy: 3.504968 LJ + 0.00501830 correction + 10.8152 Coulomb
    EXPECT_NEAR(3.504968 + 0.00501830 + 10.8152, productionEnergy.pairPotential.total(), 2.0e-4);

    // Intramolecular energy: 1.833038E+01 bond + 3.937976E+01 angle + 1.388285E+01 torsion
    EXPECT_NEAR(1.833038E+01 + 3.937976E+01 + 1.388285E+01, productionEnergy.geometry.total(), 1.0e-5);
    EXPECT_NEAR(1.833038E+01, productionEnergy.geometry.bondEnergy, 3.0e-6);
    EXPECT_NEAR(3.937976E+01, productionEnergy.geometry.angleEnergy, 3.0e-6);
    EXPECT_NEAR(1.388285E+01, productionEnergy.geometry.torsionEnergy, 5.0e-6);
}

TEST(Hexane1ForceTest, Full)
{
    TestGraph testGraph;
    auto lastNode = testGraph.createConfiguration("Box",
                                                  {
                                                      {createHexane, 1},
                                                  },
                                                  0.001);
    auto importNode = testGraph.appendImportCoordinates(
        lastNode,
        CoordinateImportFileFormat("dlpoly/hexane1/REVCON", CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));
    ASSERT_TRUE(importNode);

    // Set cell dimensions
    auto setCellNode = testGraph.findNode("SetCell");
    ASSERT_TRUE(setCellNode->setOption<Vector3>("Lengths", {30.769064857500, 46.153597286200, 30.769064857500}));

    // Adjust pair potential properties
    PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
    PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
    PairPotential::setChargeSource(PairPotential::ChargeSource::SpeciesAtoms);
    PairPotential::setRange(12.0, 1.0e-4);

    // Run the graph from the Import node to set up the configuration
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration and create a force kernel
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");
    auto kernel = testGraph.createForceKernel(cfg);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   {"dlpoly/hexane1/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 3.0e-2);
}
} // namespace UnitTest