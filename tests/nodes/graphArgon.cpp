// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/units.h"
#include "classes/configuration.h"
#include "classes/isotopologueSet.h"
#include "data/structureFactors.h"
#include "nodes/neutronSQ.h"
#include "nodes/sq.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GraphArgonTest, InitSimulation)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"Ar", 1000}}, 0.0213);

    // Get the Insert node and run the graph
    auto insertNode = testGraph.findNode("Insert-Ar");
    ASSERT_TRUE(insertNode);
    ASSERT_EQ(insertNode->run(), NodeConstants::ProcessResult::Success);

    // Check Configuration contents
    auto *cfg = insertNode->getOutputValue<Configuration *>("Configuration");
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nMolecules(), insertNode->getInputValue<Number>("Population").asInteger());
};

TEST(GraphArgonTest, AllCorrelations)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"Ar", 1000}}, 0.0213));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "dissolve2/argon/Ar_bulk_step1000.xyz"));

    // Append GR and SQ nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(true, true);

    // Set up neutron SQ
    auto neutronSQNode = testGraph.appendNeutronSQ(sqNode, "Yarnell", {{"Ar", "Ar36", 1.0}}, {"dissolve2/argon/yarnell.sq"});

    // Run the Graph from the NeutronSQ node
    ASSERT_EQ(neutronSQNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(neutronSQNode->versionIndex(), 0);

    // Check total unweighted SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);
    ASSERT_TRUE(checkData1D(unweightedSQ->total(), "UnweightedSQ", "dissolve2/argon/SQ01-UnweightedSQ-total.sq", 1, 2));

    // Check neutron weighted SQ
    auto weightedSQ = neutronSQNode->getOutputValue<PartialSet *>("WeightedSQ");
    ASSERT_TRUE(weightedSQ);
    ASSERT_TRUE(checkData1D(weightedSQ->total(), "WeightedSQ", "dissolve2/argon/NeutronSQ01-WeightedSQ-total.sq", 1, 2, 0.025));
}

} // namespace UnitTest
