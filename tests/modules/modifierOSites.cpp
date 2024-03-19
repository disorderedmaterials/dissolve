// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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

    EXPECT_TRUE(systemTest.checkData1D(
        "M//OTypes", {"dissolve/input/modifierOSites.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-8));
    EXPECT_TRUE(systemTest.checkData1D(
        "M//TotalOSites", {"dissolve/input/modifierTotalOSites.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-8));
}
TEST_F(ModifierOSitesModuleTest, TotalOSitesWBondLengths)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/modifierOSites-test2.txt"));
    ASSERT_TRUE(systemTest.dissolve().iterate(1));

    const auto &mFOdata = systemTest.dissolve().processingModuleData().search<const Data1D>("M//MFOBondLength")->get();
    const auto &mNBOdata = systemTest.dissolve().processingModuleData().search<const Data1D>("M//MNBOBondLength")->get();
    const auto &mBOdata = systemTest.dissolve().processingModuleData().search<const Data1D>("M//MBOBondLength")->get();

    // 1A M-O
    ASSERT_EQ(mFOdata.value(20), 0.5);
    // 2A M-O
    ASSERT_EQ(mFOdata.value(40), 0.5);

    // 1.5A M-O
    ASSERT_EQ(mNBOdata.value(30), 0.125);
    ASSERT_EQ(mNBOdata.value(40), 0.5);
    // sqrt(5) M-O
    ASSERT_EQ(mNBOdata.value(44), 0.25);
    // 3 M-O
    ASSERT_EQ(mNBOdata.value(60), 0.125);

    ASSERT_NEAR(mBOdata.value(20), 0.33333333333, 1e-8);
    ASSERT_NEAR(mBOdata.value(40), 0.66666666666, 1e-8);
}

} // namespace UnitTest
