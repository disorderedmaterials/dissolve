// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/siteRDF.h"
#include "classes/speciesSites.h"
#include "io/import/trajectory.h"
#include "math/rangedVector3.h"
#include "nodes/importConfigurationTrajectory.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>
#include <memory>

namespace UnitTest
{
class SiteRDFNodeTest : public ::testing::Test
{
    public:
    SiteRDFNodeTest() = default;
    ~SiteRDFNodeTest() = default;

    protected:
    // Import trajectpry into iterable graph
    bool importTrajectory(const std::string &name, TrajectoryImportFileFormat::TrajectoryImportFormat format)
    {
        if (!(cfgTrajImporter() && iterator()))
            return false;

        cfgTrajImporter()->setOption<std::string>("FilePath", name);
        cfgTrajImporter()->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>("FileFormat", format);

        return iterator()->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"});
    }
    // Return pointer to ImportConfiguationTrajectory node
    ImportConfigurationTrajectoryNode *cfgTrajImporter()
    {
        return iterator()
                   ? static_cast<ImportConfigurationTrajectoryNode *>(iterator()->findNode("ImportConfigurationTrajectory"))
                   : nullptr;
    }
    // Return pointer to SiteRDF node
    SiteRDFNode *siteRDF(const std::string &name)
    {
        return iterator() ? static_cast<SiteRDFNode *>(iterator()->findNode("SiteRDF//" + name)) : nullptr;
    }
    // Return pointer to IterableGraph node
    IterableGraph *iterator() { return static_cast<IterableGraph *>(root()->findNode("Iterator")); }
    // Return graph water species
    const Species *water() { return root()->findNode("Water")->getOutputValue<const Species *>("Species"); }
    // Return pointer to DissolveGraph
    DissolveGraph *root() { return &(testData_->graphRoot); }
    // create graph
    void createGraph(const std::string &trajectoryFilename, TrajectoryImportFileFormat::TrajectoryImportFormat format)
    {
        // Create graph test data, resetting unique pointer if necessary
        if (!testData_.get())
            testData_.reset();

        testData_ = std::make_unique<GraphTestData>();

        // Add iterator
        ASSERT_TRUE(dynamic_cast<IterableGraph *>(root()->createNode("Iterator", "Iterator")));

        // Create water graph
        createWaterGraph(root(), 267);

        // Create a dynamic input from the graph's existing Insert node
        EXPECT_TRUE(root()->addEdge({"Insert", "Configuration", "Iterator", "Configuration"}));

        // Create an import configuration trajectory node
        ASSERT_TRUE(iterator()->createNode("ImportConfigurationTrajectory", "ImportConfigurationTrajectory"));

        // Import water trajectory
        ASSERT_TRUE(importTrajectory(trajectoryFilename, format));

        // Create site RDF nodes
        ASSERT_TRUE(iterator()->createNode("SiteRDF", "SiteRDF//O-O"));
        ASSERT_TRUE(iterator()->createNode("SiteRDF", "SiteRDF//H1-H2"));
        ASSERT_TRUE(iterator()->createNode("SiteRDF", "SiteRDF//COM-COM"));

        ASSERT_TRUE(iterator()->addEdge({"ImportConfigurationTrajectory", "Configuration", "SiteRDF//O-O", "Configuration"}));
        ASSERT_TRUE(iterator()->addEdge({"ImportConfigurationTrajectory", "Configuration", "SiteRDF//H1-H2", "Configuration"}));
        ASSERT_TRUE(
            iterator()->addEdge({"ImportConfigurationTrajectory", "Configuration", "SiteRDF//COM-COM", "Configuration"}));
    }

    // Root test data
    std::unique_ptr<GraphTestData> testData_;
};

TEST_F(SiteRDFNodeTest, Water)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    // Set options
    ASSERT_TRUE(siteRDF("O-O"));
    ASSERT_TRUE(siteRDF("H1-H2"));
    ASSERT_TRUE(siteRDF("COM-COM"));
    ASSERT_TRUE(iterator());

    // O-O
    ASSERT_TRUE(siteRDF("O-O")->setOption<SpeciesSites>("SiteA", {{water()->findSite("O")}}));
    ASSERT_TRUE(siteRDF("O-O")->setOption<SpeciesSites>("SiteB", {{water()->findSite("O")}}));
    ASSERT_TRUE(siteRDF("O-O")->setOption<RangedVector3>("DistanceRange", {{0.0, 20.0, 0.01}}));
    ASSERT_TRUE(siteRDF("O-O")->setOption("ExcludeSameMolecule", true));
    ASSERT_TRUE(siteRDF("O-O")->setOption("RangeBEnabled", true));

    // H1-H2
    ASSERT_TRUE(siteRDF("H1-H2")->setOption<SpeciesSites>("SiteA", {{water()->findSite("H1")}}));
    ASSERT_TRUE(siteRDF("H1-H2")->setOption<SpeciesSites>("SiteB", {{water()->findSite("H2")}}));
    ASSERT_TRUE(siteRDF("H1-H2")->setOption<RangedVector3>("DistanceRange", {{0.0, 20.0, 0.01}}));
    ASSERT_TRUE(siteRDF("H1-H2")->setOption("ExcludeSameMolecule", true));
    ASSERT_TRUE(siteRDF("H1-H2")->setOption("RangeBEnabled", true));

    // COM-COM
    ASSERT_TRUE(siteRDF("COM-COM")->setOption<SpeciesSites>("SiteA", {{water()->findSite("COM")}}));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption<SpeciesSites>("SiteB", {{water()->findSite("COM")}}));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption<RangedVector3>("DistanceRange", {{0.0, 20.0, 0.05}}));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption("ExcludeSameMolecule", true));
    ASSERT_TRUE(siteRDF("COM-COM")->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    ASSERT_EQ(iterator()->run(), NodeConstants::ProcessResult::Success);

    // O-O RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        siteRDF("O-O")->dataRDF(), "RDF(OW-OW)_RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // H1-H2 RDF, excluding intramolecular
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        siteRDF("H1-H2")->dataRDF(), "RDF(H1-H2)_RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));

    // COM-COM RDF
    /*
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        siteRDF("COM-COM")->dataRDF(), "RDF(COM-COM)_RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));
    */
}

} // namespace UnitTest