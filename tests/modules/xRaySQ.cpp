// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class XRaySQModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(XRaySQModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/correlations-waterXRay.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    // Total F(Q)
    EXPECT_TRUE(systemTest.checkData1D(
        "H2Oxray//WeightedSQ//Total",
        {"epsr25/water1000-neutron-xray/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 2.0e-4));
}

} // namespace UnitTest
