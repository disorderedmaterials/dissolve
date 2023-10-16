// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class OrientedSDFModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(OrientedSDFModuleTest, Benzene)
{
    ASSERT_NO_THROW(systemTest.setUp<TomlFailure>("dissolve/input/orientedSDF-benzene.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(80));

    EXPECT_TRUE(systemTest.checkData3D(
        "OrientedSDF(R-R)//Process3D//SDF",
        {"dlpoly/benzene181/benzene181.11.pdens.zOrient0_10", Data3DImportFileFormat::Data3DImportFormat::PDens}, 0.09));
    EXPECT_TRUE(
        systemTest.checkData3D("OrientedSDF(R-R)//Process3D//SDF", "OrientedSDF(R-R)_Analyser//Process3D//SDF", 1.0e-5));
}
} // namespace UnitTest
