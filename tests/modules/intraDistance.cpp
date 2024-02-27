// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class IntraDistanceModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(IntraDistanceModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/intraDistance.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    EXPECT_TRUE(systemTest.checkData1D(
        "D(O-H1)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.01-02.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 6.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "D(O-H2)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.02-03.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 6.0e-4));
    EXPECT_TRUE(systemTest.checkData1D(
        "D(H1-H2)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.01-03.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 6.0e-4));
}

} // namespace UnitTest
