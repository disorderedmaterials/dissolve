// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/avgmol/avgmol.h"
#include "classes/speciesSite.h"
#include "io/export/species.h"
#include "io/import/trajectory.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(AvgMolNodeTest, Water)
{
    GraphTestData data;
    createWaterGraph(&data.graphRoot, 267);

    // Create an iterator
    auto iterator = dynamic_cast<IterableGraph *>(data.graphRoot.createNode("Iterator"));
    ASSERT_TRUE(iterator);

    // Create a dynamic input from the graph's existing Insert node
    EXPECT_TRUE(data.graphRoot.addEdge({"Insert", "Configuration", "Iterator", "Configuration"}));

    // Within the iterator create an ImportTrajectory node
    auto importTrajectory = iterator->createNode("ImportConfigurationTrajectory");
    ASSERT_TRUE(importTrajectory);
    ASSERT_TRUE(importTrajectory->setOption<std::string>("FilePath", "dlpoly/water267-analysis/water-267-298K.xyz"));
    ASSERT_TRUE(importTrajectory->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>(
        "FileFormat", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ));
    ASSERT_TRUE(iterator->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"}));

    // Add average molecule module to the iterator
    auto avg = dynamic_cast<AvgMolNode *>(iterator->createNode("AvgMol"));
    ASSERT_TRUE(avg);

    auto *water = data.graphRoot.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);
    auto *configuration = data.graphRoot.findNode("Bulk")->getOutputValue<Configuration *>("Configuration");
    ASSERT_TRUE(configuration);
    SpeciesExportFileFormat exporter("test.AVERAGE");
    ASSERT_TRUE(avg->setOption("ExportCoordinates", exporter));
    auto site = water->findSite("Origin");
    ASSERT_TRUE(site);
    ASSERT_TRUE(avg->setOption("Site", water->findSite("Origin")));

    ASSERT_TRUE(iterator->addEdge({"ImportConfigurationTrajectory", "Configuration", "AvgMol", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    // Data1DExportFileFormat exporter("test.ANGLE");
    // exporter.exportData(angle->rdfBC());
    // EXPECT_TRUE(DissolveSystemTest::checkData1D(
    //     angle->rdfBC(), "B-C RDF",
    //     {"dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", Data1DImportFileFormat::Data1DImportFormat::XY},
    //     2.0e-2));
    // EXPECT_TRUE(DissolveSystemTest::checkData1D(angle->angleABC(), "A-B-C angle",
    //                                             {"dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm",
    //                                              Data1DImportFileFormat::Data1DImportFormat::XY},
    //                                             6.0e-5));
}
} // namespace UnitTest
