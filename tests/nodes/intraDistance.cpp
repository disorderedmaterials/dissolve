// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/intraDistance.h"
#include "classes/speciesSites.h"
#include "io/import/trajectory.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
TEST(IntraDistanceNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);

    // Create iterable graph
    ASSERT_TRUE(testGraph.appendNode("Iterator", "Iterator"));
    auto iterator = testGraph.head<IterableGraph>();

    // Create a dynamic input from the graph's existing Insert node
    EXPECT_TRUE(testGraph.addEdge({"Insert-Water", "Configuration", "Iterator", "Configuration"}));

    // Within the iterator create an ImportTrajectory node
    auto importTrajectory = iterator->createNode("ImportConfigurationTrajectory");
    ASSERT_TRUE(importTrajectory);
    ASSERT_TRUE(importTrajectory->setOption<std::string>("FilePath", "dlpoly/water267-analysis/water-267-298K.xyz"));
    ASSERT_TRUE(importTrajectory->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "FileFormat", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ));
    ASSERT_TRUE(iterator->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"}));

    // Add the analysis module to the iterator
    auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);
    const Vector3 distanceRange = {0, 2, 0.01};

    auto dOH1 = static_cast<IntraDistanceNode *>(iterator->createNode("IntraDistance", "D(O-H1)"));
    ASSERT_TRUE(dOH1);
    ASSERT_TRUE(dOH1->setOption<SpeciesSites>("SiteA", {{water->findSite("O")}}));
    ASSERT_TRUE(dOH1->setOption<SpeciesSites>("SiteB", {{water->findSite("H1")}}));
    ASSERT_TRUE(dOH1->setOption<Vector3>("DistanceRange", distanceRange));
    ASSERT_TRUE(iterator->addEdge({"ImportConfigurationTrajectory", "Configuration", "D(O-H1)", "Configuration"}));

    auto dOH2 = static_cast<IntraDistanceNode *>(iterator->createNode("IntraDistance", "D(O-H2)"));
    ASSERT_TRUE(dOH2);
    ASSERT_TRUE(dOH2->setOption<SpeciesSites>("SiteA", {{water->findSite("O")}}));
    ASSERT_TRUE(dOH2->setOption<SpeciesSites>("SiteB", {{water->findSite("H2")}}));
    ASSERT_TRUE(dOH2->setOption<Vector3>("DistanceRange", distanceRange));
    ASSERT_TRUE(iterator->addEdge({"ImportConfigurationTrajectory", "Configuration", "D(O-H2)", "Configuration"}));

    auto dH1H2 = static_cast<IntraDistanceNode *>(iterator->createNode("IntraDistance", "D(H1-H2)"));
    ASSERT_TRUE(dH1H2);
    ASSERT_TRUE(dH1H2->setOption<SpeciesSites>("SiteA", {{water->findSite("H1")}}));
    ASSERT_TRUE(dH1H2->setOption<SpeciesSites>("SiteB", {{water->findSite("H2")}}));
    ASSERT_TRUE(dH1H2->setOption<Vector3>("DistanceRange", distanceRange));
    ASSERT_TRUE(iterator->addEdge({"ImportConfigurationTrajectory", "Configuration", "D(H1-H2)", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        dOH1->rdfAB(), "D(O-H1)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.01-02.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 6.0e-5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        dOH2->rdfAB(), "D(O-H2)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.02-03.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 7.0e-5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        dH1H2->rdfAB(), "D(H1-H2)//NormalisedHistogram",
        {"dlpoly/water267-analysis/water-267-298K.01-03.ij", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3}, 5.0e-5));
}

} // namespace UnitTest
