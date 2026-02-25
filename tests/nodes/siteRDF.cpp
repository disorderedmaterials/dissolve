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

    enum TestCase
    {
        Static,
        Dynamic,
        Fragments
    };

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
    // Return pointer to IterableGraph node
    IterableGraph *iterator() { return static_cast<IterableGraph *>(root()->findNode("Iterator")); }
    // Return graph water species
    const Species *water() { return root()->findNode("Water")->getOutputValue<const Species *>("Species"); }
    // Return pointer to DissolveGraph
    DissolveGraph *root() { return &(testData_->graphRoot); }
    // create graph
    void createGraph(const std::string &trajectoryFilename, TrajectoryImportFileFormat::TrajectoryImportFormat format)
    {
        testData_ = std::make_unique<GraphTestData>();

        // Add iterator
        ASSERT_TRUE(dynamic_cast<IterableGraph *>(root()->createNode("Iterator", "Iterator")));

        // Create water graph
        createWaterGraphDlPoly(root(), 267);

        // Create a dynamic input from the graph's existing Insert node
        EXPECT_TRUE(root()->addEdge({"Insert", "Configuration", "Iterator", "Configuration"}));

        // Create an import configuration trajectory node
        ASSERT_TRUE(iterator()->createNode("ImportConfigurationTrajectory", "ImportConfigurationTrajectory"));

        // Import water trajectory
        ASSERT_TRUE(importTrajectory(trajectoryFilename, format));
    }
    // create SiteRDF node
    SiteRDFNode *createSiteRDFNode(std::pair<std::string, std::string> sites, std::array<double, 3> distanceRange,
                                   bool enableSameMolecule, SiteRDFNodeTest::TestCase testCase = SiteRDFNodeTest::Static)
    {
        auto name = "SiteRDF//" + sites.first + "-" + sites.second;
        EXPECT_TRUE(iterator()->createNode("SiteRDF", name));
        auto node = iterator()->findNode(name);
        EXPECT_TRUE(iterator()->addEdge({"ImportConfigurationTrajectory", "Configuration", name, "Configuration"}));
        auto [x, y, z] = distanceRange;

        std::string a, b;
        switch (testCase)
        {
            case (TestCase::Static):
                a = sites.first;
                b = sites.second;
                break;
            case (TestCase::Dynamic):
                a = sites.first + "-dyn";
                b = sites.second + "-dyn";
                break;
            case (TestCase::Fragments):
                a = sites.first + "-frag";
                b = sites.second + "-frag";
                break;
            default:
                a = "UNKNOWN";
                b = "UNKNOWN";
                break;
        }
        auto aSite = water()->findSite(a);
        auto bSite = water()->findSite(b);
        checkSiteExists(aSite);
        checkSiteExists(bSite);
        EXPECT_TRUE(node->setOption<SpeciesSites>("SiteA", {{aSite}}));
        EXPECT_TRUE(node->setOption<SpeciesSites>("SiteB", {{bSite}}));
        EXPECT_TRUE(node->setOption<RangedVector3>("DistanceRange", {{x, y, z}}));
        EXPECT_TRUE(node->setOption("ExcludeSameMolecule", enableSameMolecule));

        return static_cast<SiteRDFNode *>(node);
    }
    static void checkSiteExists(const SpeciesSite *site) { ASSERT_TRUE(site); }

    // Root test data
    std::unique_ptr<GraphTestData> testData_;
};

TEST_F(SiteRDFNodeTest, Water)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    auto oORDF = createSiteRDFNode({"O", "O"}, {0.0, 20.0, 0.01}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto h1H2RDF = createSiteRDFNode({"H1", "H2"}, {0.0, 20.0, 0.01}, true);
    ASSERT_TRUE(h1H2RDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM", "COM"}, {0.0, 20.0, 0.05}, true);
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    ASSERT_EQ(iterator()->run(), NodeConstants::ProcessResult::Success);

    // O-O RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        oORDF->dataRDF(), "RDF(OW-OW)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // H1-H2 RDF, excluding intramolecular
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        h1H2RDF->dataRDF(), "RDF(H1-H2)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        5.0e-3));

    // COM-COM RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        comCOMRDF->dataRDF(), "RDF(COM-COM)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));

    // Coordination numbers
    auto &[cNA, cNAData] = comCOMRDF->sumN("A");
    auto &[cNB, cNBData] = comCOMRDF->sumN("B");
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number A", cNA, 4.32359551, 6.5e-1));
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number B", cNB, 19.413049, 6.5e-1));
}

TEST_F(SiteRDFNodeTest, WaterNPT)
{
    createGraph("dlpoly/water267-npt/water-267-298K.HISf", TrajectoryImportFileFormat::TrajectoryImportFormat::DLPOLYFormatted);

    auto oORDF = createSiteRDFNode({"O", "O"}, {0.0, 20.0, 0.01}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto h1H2RDF = createSiteRDFNode({"H1", "H2"}, {0.0, 20.0, 0.01}, true);
    ASSERT_TRUE(h1H2RDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM", "COM"}, {0.0, 20.0, 0.05}, true);
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    ASSERT_EQ(iterator()->run(), NodeConstants::ProcessResult::Success);

    // O-O RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        oORDF->dataRDF(), "RDF(OW-OW)//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        h1H2RDF->dataRDF(), "RDF(H1-H2)//RDF",
        {"dlpoly/water267-npt/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-2));

    // COM-COM RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        comCOMRDF->dataRDF(), "RDF(COM-COM)//RDF",
        {"dlpoly/water267-npt/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 7.0e-3));
}

TEST_F(SiteRDFNodeTest, WaterDynamic)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    auto oORDF = createSiteRDFNode({"O", "O"}, {0.0, 20.0, 0.01}, true, TestCase::Dynamic);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto hHRDF = createSiteRDFNode({"H", "H"}, {0.0, 20.0, 0.01}, true, TestCase::Dynamic);
    ASSERT_TRUE(hHRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    ASSERT_EQ(iterator()->run(), NodeConstants::ProcessResult::Success);

    // O-O RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        oORDF->dataRDF(), "RDF(OW-OW)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        9.0e-3));

    // H1-H2 RDF, excluding intramolecular interactions
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        hHRDF->dataRDF(), "RDF(H-H)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_HHsum", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        3.0e-3));
}

TEST_F(SiteRDFNodeTest, WaterFragments)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    auto oORDF = createSiteRDFNode({"O", "O"}, {0.0, 20.0, 0.01}, true, TestCase::Fragments);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto hHRDF = createSiteRDFNode({"H", "H"}, {0.0, 20.0, 0.01}, true, TestCase::Fragments);
    ASSERT_TRUE(hHRDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM", "COM"}, {0.0, 20.0, 0.05}, true, TestCase::Fragments);
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator()->setOption<Number>("N", 95));
    ASSERT_EQ(iterator()->run(), NodeConstants::ProcessResult::Success);

    // O-O RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        oORDF->dataRDF(), "RDF(OW-OW)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_02_02", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // H1-H2 RDF, excluding intramolecular
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        hHRDF->dataRDF(), "RDF(H-H)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.aardf1_01_03", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2},
        1.0e-2));

    // COM-COM RDF
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        comCOMRDF->dataRDF(), "RDF(COM-COM)//RDF",
        {"dlpoly/water267-analysis/water-267-298K.rdf11", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-4));

    // Coordination numbers
    auto &[cNA, cNAData] = comCOMRDF->sumN("A");
    auto &[cNB, cNBData] = comCOMRDF->sumN("B");
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number A", cNA, 4.32359551, 6.5e-1));
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number B", cNB, 19.413049, 6.5e-1));
}

} // namespace UnitTest