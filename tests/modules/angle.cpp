// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class AngleModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(AngleModuleTest, Water)
{
    ASSERT_NO_THROW(systemTest.setUp("dissolve/input/angle.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));


    EXPECT_TRUE(systemTest.checkData1D(
        "Angle(X-H...O)//RDF(BC)",
        {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
        3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D("Angle(X-H...O)//Angle(ABC)",
                                       {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
                                        Data1DImportFileFormat::Data1DImportFormat::XY},
                                       6.0e-5));
}

} // namespace UnitTest
