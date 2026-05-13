// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/force.h"
#include "nodes/dissolve.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class BenzeneForcefieldTest : public ::testing::Test
{
    public:
    void setUp(std::string referenceCoordinates)
    {
        ASSERT_TRUE(testGraph_.createConfiguration("Box",
                                                   {
                                                       {"species/benzene.toml", 181},
                                                   },
                                                   {29.925089931, 29.925089931, 29.925089931}));
        ASSERT_TRUE(testGraph_.appendSetCoordinates("ImportDLPOLYStructure", referenceCoordinates));

        // Adjust pair potential properties
        PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
        PairPotential::setChargeSource(PairPotential::ChargeSource::AtomTypes);
        PairPotential::setRange(12.0, 1.0e-4);

        // Run the graph from the head node to set up the configuration
        ASSERT_EQ(testGraph_.fetchHead()->run(), NodeConstants::ProcessResult::Success);
        ASSERT_EQ(testGraph_.fetchHead()->versionIndex(), 0);

        // Get the configuration
        configuration_ = testGraph_.fetchHead()->getOutputValue<Configuration *>("Configuration");
    }

    protected:
    TestGraph testGraph_;
    Configuration *configuration_{nullptr};
};

TEST_F(BenzeneForcefieldTest, Energies)
{
    setUp("dlpoly/benzene181/benzene181.CONFIG");

    // Create an energy kernel
    auto kernel = testGraph_.createEnergyKernel(configuration_);

    // Check consistency between production and test energies
    auto productionEnergy = checkEnergyConsistency(kernel);

    // Interatomic energy:  -1334.653 LJ + 182.307 correction + 561.2389 Coulomb
    EXPECT_NEAR(-1334.653 + 182.307 + 561.2389, productionEnergy.pairPotential.total(), 4.0e-2);

    // Intramolecular energy: 1.833038E+01 bond + 3.937976E+01 angle + 1.388285E+01 torsion
    EXPECT_NEAR(2354.203 + 1685.309 + 1862.134, productionEnergy.geometry.total(), 9.0e-4);
    EXPECT_NEAR(2354.203, productionEnergy.geometry.bondEnergy, 5.0e-4);
    EXPECT_NEAR(1685.309, productionEnergy.geometry.angleEnergy, 5.0e-4);
    EXPECT_NEAR(1862.134, productionEnergy.geometry.torsionEnergy, 6.0e-5);
}

TEST_F(BenzeneForcefieldTest, Forces)
{
    setUp("dlpoly/benzene181/benzene181-full.REVCON");

    // Create a force kernel
    auto kernel = testGraph_.createForceKernel(configuration_);

    // Check consistency between production and test forces
    std::vector<Vector3> pairPotentialForces, geometryForces;
    checkForceConsistency(kernel, pairPotentialForces, geometryForces);

    // Check agreement with external reference total forces
    checkReferenceForceConsistency(
        pairPotentialForces, geometryForces,
        {"dlpoly/benzene181/benzene181-full.REVCON", ForceImportFileFormat::ForceImportFormat::DLPOLY}, 0.12);
}
} // namespace UnitTest