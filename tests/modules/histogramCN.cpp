// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class HistogramCNModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(HistogramCNModuleTest, Simple)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/histogramCN-simple.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(systemTest.checkData1D(
        "CN(X-Y)//Process1D//Histogram",
        {"dissolve/input/histogramCN-simple.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 1.0e-8));
    EXPECT_TRUE(systemTest.checkData1D(
        "CN(X-Y)-Analyser//Process1D//CN",
        {"dissolve/input/histogramCN-simple.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-8));
}

TEST_F(HistogramCNModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/histogramCN-water.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    EXPECT_TRUE(systemTest.checkData1D(
        "CN(COM-COM)//Process1D//Histogram",
        {"dlpoly/water267-analysis/water-267-298K.cn11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 1.0e-8));
    EXPECT_TRUE(systemTest.checkData1D(
        "CN(COM-COM)-Analyser//Process1D//CN",
        {"dlpoly/water267-analysis/water-267-298K.cn11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 1.0e-8));
}

} // namespace UnitTest
