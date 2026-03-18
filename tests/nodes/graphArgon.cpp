// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/units.h"
#include "classes/isotopologueSet.h"
#include "data/structureFactors.h"
#include "io/import/data1D.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GraphArgonTest, InitSimulation)
{
    GraphTestData data;
    createArgonGraph(&data.graphRoot);

    // Get the Insert node and run the graph
    auto insertNode = data.graphRoot.findNode("Insert-Ar");
    ASSERT_TRUE(insertNode);
    ASSERT_EQ(insertNode->run(), NodeConstants::ProcessResult::Success);

    // Check Configuration contents
    auto *cfg = insertNode->getOutputValue<Configuration *>("Configuration");
    ASSERT_TRUE(cfg);
    EXPECT_EQ(cfg->nMolecules(), insertNode->getInputValue<Number>("Population").asInteger());
};

TEST(GraphArgonTest, AllCorrelations)
{
    GraphTestData data;
    createArgonGraph(&data.graphRoot, 1000,
                     CoordinateImportFileFormat("dissolve2/argon/Ar_bulk_step1000.xyz",
                                                CoordinateImportFileFormat::CoordinateImportFormat::XYZ));

    // Run the Graph from the NeutronSQ node
    auto neutronSQNode = data.graphRoot.findNode("Yarnell");
    ASSERT_TRUE(neutronSQNode);
    ASSERT_EQ(neutronSQNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(neutronSQNode->versionIndex(), 0);

    // Check total unweighted SQ
    auto sqNode = data.graphRoot.findNode("SQ");
    ASSERT_TRUE(sqNode);
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);
    ASSERT_TRUE(
        DissolveSystemTest::checkData1D(unweightedSQ->total(), "UnweightedSQ", {"dissolve2/argon/SQ01-UnweightedSQ-total.sq"}));

    // Check neutron weighted SQ
    auto weightedSQ = neutronSQNode->getOutputValue<PartialSet *>("WeightedSQ");
    ASSERT_TRUE(weightedSQ);
    ASSERT_TRUE(DissolveSystemTest::checkData1D(weightedSQ->total(), "WeightedSQ",
                                                {"dissolve2/argon/NeutronSQ01-WeightedSQ-total.sq"}, 0.025));
}

} // namespace UnitTest
