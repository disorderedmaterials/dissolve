// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GRNodeTest, Methods)
{
    GraphTestData data;
    createArgonGraph(&data.graphRoot, 5000);

    // Add GR node and link in configuration
    auto grNode = data.graphRoot.createNode("GR", "GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("Averaging", std::optional<Number>()));
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(data.graphRoot.addEdge({"Insert", "Configuration", "GR", "Configuration"}));

    // Calculate baseline GR with the "Test" method, a simple, serial double-loop
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::TestMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    auto rawGRBaseline = *grNode->getOutputValue<PartialSet *>("RawGR");

    // Test against simple method
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::SimpleMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    auto rawGRSimple = *grNode->getOutputValue<PartialSet *>("RawGR");
    ASSERT_EQ(grNode->versionIndex(), 1);
    ASSERT_TRUE(DissolveSystemTest::checkPartialSet(rawGRBaseline, rawGRSimple, 1.0e-8));

    // Test against cells method
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::CellsMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    auto rawGRCells = *grNode->getOutputValue<PartialSet *>("RawGR");
    ASSERT_EQ(grNode->versionIndex(), 2);
    ASSERT_TRUE(DissolveSystemTest::checkPartialSet(rawGRBaseline, rawGRCells, 1.0e-8));
}

TEST(GRNodeTest, WaterCorrelationsVsEPSR)
{
    GraphTestData data;
    createWater1000Graph(&data.graphRoot, CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                                     CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Set GR options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Run the graph
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);

    // Get the raw GR
    auto rawGR = grNode->getOutputValue<PartialSet *>("RawGR");

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "OW")), "HW-OW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "OW")), "HW-OW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.5e-2));
}
} // namespace UnitTest