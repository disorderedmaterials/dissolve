// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(WaterSPCFwEnergyTest, Water)
{
    GraphTestData data;
    createWater1000Graph(&data.graphRoot, CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                                     CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Set GR options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Run the graph from each NeutronSQ node
    auto H2O = data.graphRoot.findNode("H2O");
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    auto D2O = data.graphRoot.findNode("D2O");
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
    auto HDO = data.graphRoot.findNode("HDO");
    ASSERT_TRUE(HDO);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDO->versionIndex(), 0);

    // Check total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        D2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDO->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-5));
}

} // namespace UnitTest