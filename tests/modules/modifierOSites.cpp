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
