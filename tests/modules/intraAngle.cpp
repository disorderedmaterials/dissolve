// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class IntraAngleModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(IntraAngleModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/intraAngle.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    EXPECT_TRUE(systemTest.checkData1D(
        "A(H1-O-H2)//Angle(ABC)",
        {"dlpoly/water267-analysis/water-267-298K.01-02-03.ijk", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3},
        6.0e-4));
}

} // namespace UnitTest
