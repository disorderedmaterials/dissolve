// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class AxisAngleModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/axisAngle-benzene.txt"));
        ASSERT_TRUE(systemTest.iterateRestart(100));
    }
};

TEST_F(AxisAngleModuleTest, Benzene)
{
    const auto &data = systemTest.dissolve().processingModuleData().search<const Data1D>("AxisAngle//AxisAngle(AB)")->get();
    ASSERT_EQ(data.value(4), 1.0);
    ASSERT_EQ(std::accumulate(data.values().begin(), data.values().end(), 0.0), 1.0);
}

} // namespace UnitTest
