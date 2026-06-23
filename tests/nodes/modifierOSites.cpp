// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/modifierOSites.h"
#include "tests/testData.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(ModifierOSitesNodeTest, Simple)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"Si", 9}, {"O", 13}, {"P", 8}}, {30.0, 30.0, 30.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/modifierOSites.xyz"));

    // Grab species pointers
    auto *Si = testGraph.findNode("Si")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(Si);
    auto *O = testGraph.findNode("O")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(O);
    auto *P = testGraph.findNode("P")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(P);

    // Add the analysis module to the iterator
    auto modOS = dynamic_cast<ModifierOSitesNode *>(testGraph.createNode("ModifierOSites"));
    ASSERT_TRUE(modOS);
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("Modifier", {{P->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("BondingOxygen", {{O->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("NetworkFormer", {{Si->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<Range>("DistanceRange", {0.0, 2.1}));
    ASSERT_TRUE(modOS->setOption<Range>("ModifierDistanceRange", {0.0, 2.1}));
    ASSERT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "ModifierOSites", "Configuration"}));

    // Run from the modifier O sites node explicitly
    ASSERT_EQ(modOS->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(modOS->versionIndex(), 0);

    auto &oTypesHisto = modOS->oxygenSitesHistogram();
    auto &oTypes = modOS->oxygenSites();
    ASSERT_EQ(oTypesHisto.averages().size(), 3);
    ASSERT_EQ(oTypes.nValues(), 3);
    EXPECT_EQ(oTypesHisto.averages().at(0), 2);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(0), 0.0);
    EXPECT_DOUBLE_EQ(oTypes.value(0), 2.0 / 8.0);
    EXPECT_EQ(oTypesHisto.averages().at(1), 3);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(1), 1.0);
    EXPECT_DOUBLE_EQ(oTypes.value(1), 3.0 / 8.0);
    EXPECT_EQ(oTypesHisto.averages().at(2), 3);
    EXPECT_DOUBLE_EQ(oTypes.xAxis(2), 2.0);
    EXPECT_DOUBLE_EQ(oTypes.value(2), 3.0 / 8.0);

    auto &oTotalHisto = modOS->totalOxygensHistogram();
    auto &oTotal = modOS->totalOxygens();
    ASSERT_EQ(oTotalHisto.averages().size(), 1);
    ASSERT_EQ(oTotal.nValues(), 1);
    EXPECT_EQ(oTotalHisto.averages().at(1), 8);
    EXPECT_DOUBLE_EQ(oTotal.xAxis(0), 1.0);
    EXPECT_DOUBLE_EQ(oTotal.value(0), 1.0);
}

TEST(ModifierOSitesNodeTest, Simple2)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"Si", 5}, {"O", 9}, {"P", 10}}, {30.0, 30.0, 30.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/modifierOSites2.xyz"));

    // Grab species pointers
    auto *Si = testGraph.findNode("Si")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(Si);
    auto *O = testGraph.findNode("O")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(O);
    auto *P = testGraph.findNode("P")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(P);

    // Add the analysis module to the iterator
    auto modOS = dynamic_cast<ModifierOSitesNode *>(testGraph.createNode("ModifierOSites"));
    ASSERT_TRUE(modOS);
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("Modifier", {{P->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("BondingOxygen", {{O->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<SpeciesSites>("NetworkFormer", {{Si->findSite("Origin")}}));
    ASSERT_TRUE(modOS->setOption<Range>("DistanceRange", {0.0, 2.1}));
    ASSERT_TRUE(modOS->setOption<Range>("ModifierDistanceRange", {0.0, 3.1}));
    ASSERT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "ModifierOSites", "Configuration"}));

    // Run from the modifier O sites node explicitly
    ASSERT_EQ(modOS->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(modOS->versionIndex(), 0);

    auto &distanceMFO = modOS->distanceMFO();
    Interpolator distanceMFOInterpolator(distanceMFO);
    EXPECT_NEAR(distanceMFOInterpolator.y(1.025), 0.5, 1.0e-8);
    EXPECT_NEAR(distanceMFOInterpolator.y(2.025), 0.5, 1.0e-8);

    auto &distanceMNBO = modOS->distanceMNBO();
    Interpolator distanceMNBOInterpolator(distanceMNBO);
    EXPECT_NEAR(distanceMNBOInterpolator.y(1.525), 0.125, 1.0e-8);
    EXPECT_NEAR(distanceMNBOInterpolator.y(2.025), 0.5, 1.0e-8);
    EXPECT_NEAR(distanceMNBOInterpolator.y(2.225), 0.25, 1.0e-8);
    EXPECT_NEAR(distanceMNBOInterpolator.y(3.025), 0.125, 1.0e-8);

    auto &distanceMBO = modOS->distanceMBO();
    Interpolator distanceMBOInterpolator(distanceMBO);
    EXPECT_NEAR(distanceMBOInterpolator.y(1.025), 1.0 / 3.0, 1.0e-8);
    EXPECT_NEAR(distanceMBOInterpolator.y(2.025), 2.0 / 3.0, 1.0e-8);
}
} // namespace UnitTest