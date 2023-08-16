// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class AvgMolModuleWaterTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW(systemTest.setUp("../unit/modules/avgMol/water.txt"));
        ASSERT_TRUE(systemTest.iterate(95));
    }
};

TEST_F(AvgMolModuleWaterTest, AverageCoordinates)
{
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//X", {-0.83305, 0.00016, 0.83305}));
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Y", {0.00000, 0.60443, 0.00000}));
    EXPECT_TRUE(systemTest.checkSampledVector("AvgMol01//Z", {0.00000, 0.00000, 0.00000}));
}

} // namespace UnitTest
