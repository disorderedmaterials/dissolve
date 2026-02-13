// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(WaterSPCFwEnergyTest, Water)
{
    GraphTestData data;
    createWaterGraph(&data.graphRoot, 1000,
                     CoordinateImportFileFormat("dlpoly/water3000_energyForce/CONFIG",
                                                CoordinateImportFileFormat::CoordinateImportFormat::DLPOLY));

    // Add energy node
    auto energyNode = data.graphRoot.createNode("Energy");
    ASSERT_TRUE(energyNode);
    ASSERT_TRUE(data.graphRoot.addEdge({"Import", "Configuration", "Energy", "Configuration"}));

    // Run the graph from the Energy node
    ASSERT_EQ(energyNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(energyNode->versionIndex(), 0);
}

} // namespace UnitTest