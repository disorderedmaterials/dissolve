// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/compare/compare.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
using RangeErrorPair = std::pair<std::vector<Range>, std::vector<double>>;

// Water Molecule
class CompareModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW(systemTest.setUp("dissolve/input/epsr-benzene-3n.txt"));
        ASSERT_TRUE(systemTest.dissolve().iterate(1));
    }
};

TEST_F(CompareModuleTest, IntVsIntErrors)
{
    auto *compareModule = systemTest.getModule<CompareModule>("Compare01");
    ASSERT_TRUE(compareModule);

    const auto &ranges = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[0]).first;
    const auto &errors = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[0]).second;

    EXPECT_NEAR(ranges[0].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[0].maximum(), 50, 1.0e-4);
    EXPECT_NEAR(ranges[1].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[1].maximum(), 10, 1.0e-4);

    EXPECT_NEAR(errors[0], 3.0942e-1, 1.0e-4);
    EXPECT_NEAR(errors[1], 9.2657e-1, 1.0e-4);
}

TEST_F(CompareModuleTest, IntVsExtErrors)
{
    auto *compareModule = systemTest.getModule<CompareModule>("Compare01");
    ASSERT_TRUE(compareModule);

    const auto &ranges = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[1]).first;
    const auto &errors = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[1]).second;

    EXPECT_NEAR(ranges[0].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[0].maximum(), 30, 1.0e-4);
    EXPECT_NEAR(ranges[1].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[1].maximum(), 10, 1.0e-4);

    EXPECT_NEAR(errors[0], 3.0942e-1, 1.0e-4);
    EXPECT_NEAR(errors[1], 9.2657e-1, 1.0e-4);
}

TEST_F(CompareModuleTest, ExtVsExtErrors)
{
    auto *compareModule = systemTest.getModule<CompareModule>("Compare01");
    ASSERT_TRUE(compareModule);

    const auto &ranges = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[2]).first;
    const auto &errors = compareModule->data1dSourcesErrors().at(&compareModule->data1dSources()[2]).second;

    EXPECT_NEAR(ranges[0].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[0].maximum(), 30, 1.0e-4);
    EXPECT_NEAR(ranges[1].minimum(), 0, 1.0e-4);
    EXPECT_NEAR(ranges[1].maximum(), 10, 1.0e-4);

    EXPECT_NEAR(errors[0], 7.0069e-3, 1.0e-4);
    EXPECT_NEAR(errors[1], 2.0418e-2, 1.0e-4);
}

TEST_F(CompareModuleTest, IntVsIntDelta)
{
    EXPECT_TRUE(systemTest.checkData1D(
        "Compare01//Pair1_Delta", {"dissolve/compare/intvsint.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(CompareModuleTest, IntVsExtDelta)
{
    EXPECT_TRUE(systemTest.checkData1D(
        "Compare01//Pair2_Delta", {"dissolve/compare/intvsext.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

TEST_F(CompareModuleTest, ExtVsExtDelta)
{
    EXPECT_TRUE(systemTest.checkData1D(
        "Compare01//Pair3_Delta", {"dissolve/compare/extvsext.dat", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}));
}

} // namespace UnitTest
