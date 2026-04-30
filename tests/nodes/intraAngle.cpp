// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/intraAngle.h"
#include "classes/speciesSites.h"
#include "io/import/trajectory.h"
#include "math/rangedVector3.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <vector>

namespace UnitTest
{
TEST(IntraAngleNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{[]() { return TestGraph::loadTOMLSpecies("species/water-dlpoly.toml"); }, 267}},
                                  0.1);

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
    auto intraAngle = dynamic_cast<IntraAngleNode *>(iterator->createNode("IntraAngle"));
    ASSERT_TRUE(intraAngle);
    auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);
    ASSERT_TRUE(intraAngle->setOption<SpeciesSites>("SiteA", {{water->findSite("H1")}}));
    ASSERT_TRUE(intraAngle->setOption<SpeciesSites>("SiteB", {{water->findSite("O")}}));
    ASSERT_TRUE(intraAngle->setOption<SpeciesSites>("SiteC", {{water->findSite("H2")}}));
    ASSERT_TRUE(intraAngle->setOption<Vector3>("AngleRange", {0.0, 180.0, 0.1}));
    ASSERT_TRUE(iterator->addEdge({"ImportConfigurationTrajectory", "Configuration", "IntraAngle", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        intraAngle->intraAngleData(), "A(H1-O-H2)//Angle(ABC)",
        {"dlpoly/water267-analysis/water-267-298K.01-02-03.ijk", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 3},
        2.0e-4));
}

} // namespace UnitTest
