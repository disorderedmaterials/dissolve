// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/integerHistogram1D.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class ModifierOSitesModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(ModifierOSitesModuleTest, Simple)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/modifierOSites.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    auto &oTypesSitesHisto =
        systemTest.dissolve().processingModuleData().retrieve<IntegerHistogram1D>("OTypeSitesHistogram", "M");
    auto &oTypes = systemTest.dissolve().processingModuleData().retrieve<Data1D>("OTypes", "M");
    ASSERT_EQ(oTypesSitesHisto.averages().size(), 3);
    ASSERT_EQ(oTypes.nValues(), 3);
    EXPECT_EQ(oTypesSitesHisto.averages().at(0), 2);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(0), 0.0);
    EXPECT_DOUBLE_EQ(oTypes.value(0), 2.0 / 8.0);
    EXPECT_EQ(oTypesSitesHisto.averages().at(1), 3);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(1), 1.0);
    EXPECT_DOUBLE_EQ(oTypes.value(1), 3.0 / 8.0);
    EXPECT_EQ(oTypesSitesHisto.averages().at(2), 3);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(2), 2.0);
    EXPECT_DOUBLE_EQ(oTypes.value(2), 3.0 / 8.0);

    auto &oTotalHisto = systemTest.dissolve().processingModuleData().retrieve<IntegerHistogram1D>("TotalOHistogram", "M");
    auto &oTotal = systemTest.dissolve().processingModuleData().retrieve<Data1D>("TotalOSites", "M");
    ASSERT_EQ(oTotalHisto.averages().size(), 1);
    ASSERT_EQ(oTotal.nValues(), 1);
    EXPECT_EQ(oTotalHisto.averages().at(1), 8);
    EXPECT_DOUBLE_EQ(oTotal.xAxis(0), 1.0);
    EXPECT_DOUBLE_EQ(oTotal.value(0), 1.0);
}
TEST_F(ModifierOSitesModuleTest, TotalOSitesWBondLengths)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/modifierOSites-test2.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    EXPECT_TRUE(systemTest.checkData1D(
        "M//MFOBondLength", {"dissolve/input/FOBondLengthTest.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-8));
    EXPECT_TRUE(systemTest.checkData1D(
        "M//MNBOBondLength", {"dissolve/input/NBOBondLengthTest.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-8));
    EXPECT_TRUE(systemTest.checkData1D(
        "M//MBOBondLength", {"dissolve/input/BOBondLengthTest.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-8));
}

} // namespace UnitTest
