// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "math/error.h"
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

} // namespace UnitTest