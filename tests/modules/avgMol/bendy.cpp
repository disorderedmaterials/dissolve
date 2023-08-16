// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "unit/data/systemTest.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class AvgMolModuleBendyTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override { ASSERT_NO_THROW(systemTest.setUp("../unit/modules/avgMol/bendy.txt")); }

    void checkCoordinates()
    {
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//X", {2.0, 0.0, 2.0}, 1.0e-10, Error::ErrorType::RMSEError));
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Y", {0.0, 0.0, 2.828427}, 1.0e-7, Error::ErrorType::RMSEError));
        EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Z", {0.0, 0.0, 0.0}, 1.0e-10, Error::ErrorType::RMSEError));
    }
};

TEST_F(AvgMolModuleBendyTest, AverageCoordinates)
{
    ASSERT_TRUE(systemTest.iterate(201));

    checkCoordinates();
}

TEST_F(AvgMolModuleBendyTest, Restart)
{
    ASSERT_NO_THROW(systemTest.loadRestart("../unit/modules/avgMol/bendy.124.restart"));
    ASSERT_TRUE(systemTest.iterate(77));

    checkCoordinates();
}

} // namespace UnitTest
