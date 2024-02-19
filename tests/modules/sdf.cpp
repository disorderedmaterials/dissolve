// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
// Water Molecule
class SDFModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(SDFModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/sdf-water.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    EXPECT_TRUE(systemTest.checkData3D(
        "SDF(O)//SDF", {"dlpoly/water267-analysis/water-267-298K.11.pdens", Data3DImportFileFormat::Data3DImportFormat::PDens},
        0.12));
}

TEST_F(SDFModuleTest, Benzene)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/sdf-benzene.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(80));

    EXPECT_TRUE(systemTest.checkData3D(
        "SDF(R-R)//SDF", {"dlpoly/benzene181/benzene181.11.pdens", Data3DImportFileFormat::Data3DImportFormat::PDens}, 0.3));
    EXPECT_TRUE(systemTest.checkData3D("SDF(R-R)//SDF", "SDF(R-R)_Analyser//Process3D//SDF", 1.0e-5));
}
} // namespace UnitTest
