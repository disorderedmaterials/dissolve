// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/histogramCN.h"
#include "nodes/angle.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(HistogramCNNodeTest, Simple)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"P", 9}, {"F", 28}}, {20.0, 20.0, 20.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/histogramCN-simple.xyz"));

    // Grab atomic species and set up simple sites
    auto *P = dynamic_cast<SpeciesNode *>(testGraph.findNode("P"));
    ASSERT_TRUE(P);
    auto &speciesP = P->species();

    auto *F = dynamic_cast<SpeciesNode *>(testGraph.findNode("F"));
    ASSERT_TRUE(F);
    auto &speciesF = F->species();

    // Add the analysis module to the iterator
    auto histoCN = dynamic_cast<HistogramCNNode *>(testGraph.createNode("HistogramCN"));
    ASSERT_TRUE(histoCN);
    ASSERT_TRUE(histoCN->setOption<SpeciesSites>("SiteA", {{speciesP.findSite("Origin")}}));
    ASSERT_TRUE(histoCN->setOption<SpeciesSites>("SiteB", {{speciesF.findSite("Origin")}}));
    ASSERT_TRUE(histoCN->setOption<Range>("RangeAB", {0.0, 2.0}));
    EXPECT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "HistogramCN", "Configuration"}));

    // Run from the HistogranCN node
    ASSERT_EQ(histoCN->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(histoCN->versionIndex(), 0);

    // Bin indices / CNs and their populations
    std::vector<std::pair<int, int>> referenceData = {{0, 1}, {1, 1}, {2, 1}, {3, 2}, {4, 2}, {5, 1}, {6, 1}};
    auto sum = std::accumulate(referenceData.begin(), referenceData.end(), 0,
                               [](const auto &acc, const auto &pair) { return acc + pair.second; });

    auto data = histoCN->cn();
    for (auto &[bin, population] : referenceData)
        EXPECT_DOUBLE_EQ(data.value(bin), population / double(sum));
}

TEST(HistogramCNNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, {20.0, 20.0, 20.0}));

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
    EXPECT_TRUE(iterator);

    // Get the water species
    auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);

    // Add the analysis module to the iterator
    auto histoCN = dynamic_cast<HistogramCNNode *>(iterator->createNode("HistogramCN"));
    ASSERT_TRUE(histoCN);
    ASSERT_TRUE(histoCN->setOption<SpeciesSites>("SiteA", {{water->findSite("COM")}}));
    ASSERT_TRUE(histoCN->setOption<SpeciesSites>("SiteB", {{water->findSite("COM")}}));
    ASSERT_TRUE(histoCN->setOption<Range>("RangeAB", {0.0, 3.4}));
    EXPECT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "HistogramCN", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    // Coordination numbers and bin populations
    std::vector<std::pair<int, int>> referenceData = {{0, 0},    {1, 8},    {2, 167}, {3, 1887}, {4, 9567},
                                                      {5, 8991}, {6, 3745}, {7, 864}, {8, 128},  {9, 8}};
    auto sum = std::accumulate(referenceData.begin(), referenceData.end(), 0,
                               [](const auto &acc, const auto &pair) { return acc + pair.second; });

    auto data = histoCN->cn();
    for (auto &[bin, population] : referenceData)
        EXPECT_DOUBLE_EQ(data.value(bin), population / double(sum));
}

} // namespace UnitTest