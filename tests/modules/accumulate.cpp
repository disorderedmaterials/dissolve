// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
class AccumulateModuleTest : public ::testing::Test
{
    protected:
    DissolveSystemTest systemTest;

    void SetUp() override
    {
        ASSERT_NO_THROW(systemTest.setUp("dissolve/input/accumulate.txt"));
        ASSERT_TRUE(systemTest.dissolve().iterate(20));
    }
};

TEST_F(AccumulateModuleTest, Water)
{
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//Total", "AveragedRDF//UnweightedGR//Total"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//HW-HW//Full", "AveragedRDF//UnweightedGR//HW-HW//Full"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//HW-OW//Full", "AveragedRDF//UnweightedGR//HW-OW//Full"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//OW-OW//Full", "AveragedRDF//UnweightedGR//OW-OW//Full"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//OW-OW//Full", "RDF(OW-OW)//Process1D//RDF"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//OW-OW//Unbound", "RDF(OW-OW)//Process1D//RDF"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//HW-OW//Unbound", "RDF(O-H1_H2)//Process1D//RDF"));
    EXPECT_TRUE(systemTest.checkData1D("Accumulate01//GR01//HW-HW//Unbound", "RDF(H1-H2)//Process1D//RDF"));
}

} // namespace UnitTest
