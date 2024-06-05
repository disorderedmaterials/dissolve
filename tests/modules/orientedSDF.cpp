// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/orientedSDF-benzene.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(80));

    EXPECT_TRUE(systemTest.checkData3D(
        "OrientedSDF(R-R)//SDF",
        {"dlpoly/benzene181/benzene181.11.pdens.zOrient0_10", Data3DImportFileFormat::Data3DImportFormat::PDens}, 0.09));
}
} // namespace UnitTest
