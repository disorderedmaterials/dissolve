// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/energy.h"
#include "nodes/gr/gr.h"
#include "nodes/dissolve.h"
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

    // Run the graph from the Import node to set up the configuration
    auto importNode = data.graphRoot.findNode("Import");
    ASSERT_TRUE(importNode);
    ASSERT_EQ(importNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(importNode->versionIndex(), 0);

    // Get the configuration
    auto cfg = importNode->getOutputValue<Configuration *>("Configuration");

    // Check consistency between production and test energies
    DissolveSystemTest::checkEnergyConsistency(cfg, data.graphRoot.dissolveGraph()->prepareEnergyKernel(cfg));
}

} // namespace UnitTest