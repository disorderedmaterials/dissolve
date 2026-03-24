// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/avgmol/avgmol.h"
#include "classes/speciesSite.h"
#include "io/export/species.h"
#include "io/import/trajectory.h"
#include "math/vector3.h"
#include "mermaid.h"
#include "nodes/importConfigurationTrajectory.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/speciesData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{

class AvgMolNodeTest : public ::testing::Test
{
    public:
    AvgMolNodeTest() = default;
    ~AvgMolNodeTest() = default;

    protected:
    GraphTestData testData_;
    IterableGraph *iterator_{nullptr};
    ImportConfigurationTrajectoryNode *trajectoryImport_{nullptr};
    const Species *water_;

    protected:
    // Create graph
    void createGraph(const std::string &trajectoryFilename, TrajectoryImportFileFormat::TrajectoryImportFormat format)
    {
        // Create the water configuration
        createConfiguration(&testData_.graphRoot, "Box", {{createWaterDLPoly, 267}}, 0.1);

        // Add iterator
        iterator_ = dynamic_cast<IterableGraph *>(testData_.graphRoot.createNode("Iterator", "Iterator"));
        ASSERT_TRUE(iterator_);

        // Grab the water species for convenience
        auto waterNode = testData_.graphRoot.findNode("Water");
        ASSERT_TRUE(waterNode);
        water_ = waterNode->getOutputValue<const Species *>("Species");
        ASSERT_TRUE(waterNode);

        // Create a dynamic input from the graph's existing Insert node
        EXPECT_TRUE(testData_.graphRoot.addEdge({"Insert-Water", "Configuration", "Iterator", "Configuration"}));

        // Create an import configuration trajectory node
        trajectoryImport_ =
            dynamic_cast<ImportConfigurationTrajectoryNode *>(iterator_->createNode("ImportConfigurationTrajectory"));
        ASSERT_TRUE(trajectoryImport_);
        ASSERT_TRUE(trajectoryImport_->setOption<std::string>("FilePath", trajectoryFilename));
        ASSERT_TRUE(trajectoryImport_->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>("FileFormat", format));
        ASSERT_TRUE(iterator_->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"}));
    }
};

TEST_F(AvgMolNodeTest, Water)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    // Add average molecule module to the iterator
    auto avg = dynamic_cast<AvgMolNode *>(iterator_->createNode("AvgMol", "Average"));
    ASSERT_TRUE(avg);
    std::cout << std::get<0>(*iterator_->outputs().begin()) << std::endl;
    ASSERT_TRUE(iterator_->addEdge({"ImportConfigurationTrajectory", "Configuration", "Average", "Configuration"}));

    SpeciesExportFileFormat exporter("test.AVERAGE");
    ASSERT_TRUE(avg->setOption("ExportCoordinates", exporter));
    auto site = water_->findSite("origin");
    ASSERT_TRUE(site);
    ASSERT_TRUE(avg->setOption("Site", water_->findSite("origin")));

    // // Run from the iterator node explicitly
    ASSERT_TRUE(iterator_->setOption<Number>("N", 1));
    ASSERT_EQ(iterator_->run(), NodeConstants::ProcessResult::Success);

    // // auto result = avg->findOption("Average Species")->get<const Species*>();
    auto result = avg->findOutput("Average Species")->get<const Species *>();
    ASSERT_TRUE(result);

    ASSERT_EQ(result->atoms().size(), 3);
    ASSERT_EQ(result->atom(0).Z(), Elements::Element::H);

    // EXPECT_NEAR(result->atom(0).r().x, -0.83305, 1e-8);
    // EXPECT_NEAR(result->atom(0).r().y, 0.0, 1e-8);
    // EXPECT_NEAR(result->atom(0).r().z, 0.0, 1e-8);

    // EXPECT_NEAR(result->atom(1).r().x, 0, 1e-8);
    // EXPECT_NEAR(result->atom(1).r().y, 0, 1e-8);
    // EXPECT_NEAR(result->atom(1).r().z, 0, 1e-8);

    // EXPECT_NEAR(result->atom(0).r().x, -0.83305, 1e-8);
    // EXPECT_NEAR(result->atom(1).r().x, 0.00016, 1e-8);
    // EXPECT_NEAR(result->atom(2).r().x, 0.83305, 1e-8);
    // EXPECT_NEAR(result->atom(0).r().y, 0.0, 1e-8);
    // EXPECT_NEAR(result->atom(1).r().y, 0.60443, 1e-8);
    // EXPECT_NEAR(result->atom(2).r().y, 0.0, 1e-8);
    // EXPECT_NEAR(result->atom(0).r().z, 0.0, 1e-8);
    // EXPECT_NEAR(result->atom(1).r().z, 0.0, 1e-8);
    // EXPECT_NEAR(result->atom(2).r().z, 0.0, 1e-8);

    exportMermaidGraph(testData_.graphRoot);
}
} // namespace UnitTest
