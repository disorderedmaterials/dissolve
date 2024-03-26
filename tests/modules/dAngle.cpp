// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class DAngleModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;
};

TEST_F(DAngleModuleTest, Water)
{
    ASSERT_NO_THROW_VERBOSE(systemTest.setUp("dissolve/input/dAngle.txt"));
    ASSERT_TRUE(systemTest.iterateRestart(95));

    EXPECT_TRUE(systemTest.checkData1D(
        "DAngle(X-H...O)//RDF(BC)",
        {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
        3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D(
        "DAngle(X-H...O)-Analyser//Process1D//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
        3.0e-3));
    EXPECT_TRUE(systemTest.checkData1D("DAngle(X-H...O)//Angle(ABC)",
                                       {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
                                        Data1DImportFileFormat::Data1DImportFormat::XY},
                                       6.0e-5));
    EXPECT_TRUE(systemTest.checkData1D("DAngle(X-H...O)-Analyser//Process1D//Angle",
                                       {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
                                        Data1DImportFileFormat::Data1DImportFormat::XY},
                                       6.0e-5));
}

} // namespace UnitTest
