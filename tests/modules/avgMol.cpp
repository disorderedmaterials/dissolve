// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
// Water Molecule
class AvgMolModuleWaterTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW(systemTest.setUp("dissolve/input/avgMol-water.txt"));
        ASSERT_TRUE(systemTest.iterateRestart(95));
    }
};

TEST_F(AvgMolModuleWaterTest, AverageCoordinates)
{
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//X", {-0.83305, 0.00016, 0.83305}));
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Y", {0.00000, 0.60443, 0.00000}));
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Z", {0.00000, 0.00000, 0.00000}));
}

// BeNDy Molecule
class AvgMolModuleBendyTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("dissolve/input/avgMol-bendy.txt")); }

    void checkCoordinates()
    {
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//X", {2.0, 0.0, 2.0}, 1.0e-10, Error::ErrorType::RMSEError));
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Y", {0.0, 0.0, 2.828427}, 1.0e-7, Error::ErrorType::RMSEError));
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Z", {0.0, 0.0, 0.0}, 1.0e-10, Error::ErrorType::RMSEError));
    }
};

TEST_F(AvgMolModuleBendyTest, AverageCoordinates)
{
    ASSERT_TRUE(systemTest.iterateRestart(201));

    checkCoordinates();
}

} // namespace UnitTest
