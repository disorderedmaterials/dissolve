// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/force.h"
#include "nodes/dissolve.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class HexaneForcefieldTest : public ::testing::Test
{
    public:
    void setUp(int nMols, std::string_view referenceCoordinates)
    {
        auto lastNode = testGraph_.createConfiguration("Box",
                                                       {
                                                           {createHexane, nMols},
                                                       },
                                                       {30.769064857500, 46.153597286200, 30.769064857500});
        auto importNode = testGraph_.appendImportCoordinates(
            lastNode,
            CoordinateImportFileFormat(referenceCoordinates, CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));
        ASSERT_TRUE(importNode);

        // Adjust pair potential properties
        PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
        PairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme::NoCoulombTruncation);
        PairPotential::setChargeSource(PairPotential::ChargeSource::SpeciesAtoms);
        PairPotential::setRange(12.0, 1.0e-4);

        // Run the graph from the Import node to set up the configuration
        ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
        ASSERT_EQ(importNode->versionIndex(), 0);

        // Get the configuration
        configuration_ = importNode->getOutputValue<Configuration *>("Configuration");
    }

    protected:
    TestGraph testGraph_;
    Configuration *configuration_{nullptr};
};

TEST_F(HexaneForcefieldTest, Energies1)
{
    setUp(1, "dlpoly/hexane1/REVCON");

    // Create an energy kernel
    auto kernel = testGraph_.createEnergyKernel(configuration_);

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

TEST_F(HexaneForcefieldTest, Forces1)
{
    setUp(1, "dlpoly/hexane1/REVCON");

    // Create a force kernel
    auto kernel = testGraph_.createForceKernel(configuration_);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   {"dlpoly/hexane1/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 3.0e-2);
}

TEST_F(HexaneForcefieldTest, Energies2)
{
    setUp(2, "dlpoly/hexane2/REVCON");

    // Create an energy kernel
    auto kernel = testGraph_.createEnergyKernel(configuration_);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Interatomic energy: 5.200344 LJ + 0.0200732 correction + 21.52276 Coulomb
    EXPECT_NEAR(5.200344 + 0.0200732 + 21.52276, productionEnergy.pairPotential.total(), 3.0e-4);

    // Intramolecular energy: 33.91651 bond + 72.22392 angle + 24.49599 torsion
    EXPECT_NEAR(33.91651 + 72.22392 + 24.49599, productionEnergy.geometry.total(), 1.0e-5);
    EXPECT_NEAR(33.91651, productionEnergy.geometry.bondEnergy, 3.0e-6);
    EXPECT_NEAR(72.22392, productionEnergy.geometry.angleEnergy, 3.0e-6);
    EXPECT_NEAR(24.49599, productionEnergy.geometry.torsionEnergy, 5.0e-6);
}

TEST_F(HexaneForcefieldTest, Forces2)
{
    setUp(2, "dlpoly/hexane2/REVCON");

    // Create a force kernel
    auto kernel = testGraph_.createForceKernel(configuration_);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   {"dlpoly/hexane2/REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 3.0e-2);
}

TEST_F(HexaneForcefieldTest, Energies200)
{
    setUp(200, "dlpoly/hexane200/full.REVCON");

    // Create an energy kernel
    auto kernel = testGraph_.createEnergyKernel(configuration_);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Interatomic energy: -5124.720 LJ + 200.732 correction + 2020.063 Coulomb
    EXPECT_NEAR(-5124.720 + 200.732 + 2020.063, productionEnergy.pairPotential.total(), 5.0e-2);

    // Intramolecular energy: 3541.913 bond + 5772.231 angle + 2173.978 torsion
    EXPECT_NEAR(3541.913 + 5772.231 + 2173.978, productionEnergy.geometry.total(), 6.0e-4);
    EXPECT_NEAR(3541.913, productionEnergy.geometry.bondEnergy, 3.0e-4);
    EXPECT_NEAR(5772.231, productionEnergy.geometry.angleEnergy, 3.0e-4);
    EXPECT_NEAR(2173.978, productionEnergy.geometry.torsionEnergy, 5.0e-4);
}

TEST_F(HexaneForcefieldTest, Forces200)
{
    setUp(200, "dlpoly/hexane200/full.REVCON");

    // Create a force kernel
    auto kernel = testGraph_.createForceKernel(configuration_);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   {"dlpoly/hexane200/full.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.2);

    // Check agreement with external reference total bound forces only
    std::ranges::fill(pairPotentialForces, Vector3());
    checkReferenceForceConsistency(pairPotentialForces, geometryForces,
                                   {"dlpoly/hexane200/bound.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 1.0e-7);
}
} // namespace UnitTest