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
    TestGraph testGraph_;
    IterableGraph *iterator_{nullptr};
    ImportConfigurationTrajectoryNode *trajectoryImport_{nullptr};
    const Species *water_;

    protected:
    // Create graph
    void createGraph(const std::string &trajectoryFilename, TrajectoryImportFileFormat::TrajectoryImportFormat format)
    {
        // Create the water configuration
        testGraph_.createConfiguration("Box", {{[]() { return loadTOMLSpecies("species/water-dlpoly.toml"); }, 267}}, 0.1);

        // Add iterator
        EXPECT_TRUE(testGraph_.appendNode("Iterator", "Iterator"));
        iterator_ = testGraph_.head<IterableGraph>();

        // Grab the water species for convenience
        auto waterNode = testGraph_.findNode("Water");
        ASSERT_TRUE(waterNode);
        water_ = waterNode->getOutputValue<const Species *>("Species");
        ASSERT_TRUE(waterNode);

        // Create a dynamic input from the graph's existing Insert node
        EXPECT_TRUE(testGraph_.addEdge({"Insert-Water", "Configuration", "Iterator", "Configuration"}));

        // Create an import configuration trajectory node
        trajectoryImport_ =
            dynamic_cast<ImportConfigurationTrajectoryNode *>(iterator_->createNode("ImportConfigurationTrajectory"));
        ASSERT_TRUE(trajectoryImport_);
        ASSERT_TRUE(trajectoryImport_->setOption<std::string>("FilePath", trajectoryFilename));
        ASSERT_TRUE(trajectoryImport_->setOption<TrajectoryImportFileFormat::TrajectoryImportFormat>("FileFormat", format));
        ASSERT_TRUE(iterator_->addEdge({"Inputs", "Configuration", "ImportConfigurationTrajectory", "Configuration"}));
    }
    // Create SiteRDF node
    SiteRDFNode *createSiteRDFNode(std::pair<std::string, std::string> sites, const RangedVector3 &distanceRange,
                                   bool enableSameMolecule)
    {
        auto name = "SiteRDF//" + sites.first + "-" + sites.second;
        auto siteRDFNode = dynamic_cast<SiteRDFNode *>(iterator_->createNode("SiteRDF", name));
        EXPECT_TRUE(siteRDFNode);
        EXPECT_TRUE(iterator_->addEdge({"ImportConfigurationTrajectory", "Configuration", name, "Configuration"}));

        auto aSite = water_->findSite(sites.first);
        auto bSite = water_->findSite(sites.second);
        EXPECT_TRUE(aSite);
        EXPECT_TRUE(bSite);

        EXPECT_TRUE(siteRDFNode->setOption<SpeciesSites>("SiteA", {{aSite}}));
        EXPECT_TRUE(siteRDFNode->setOption<SpeciesSites>("SiteB", {{bSite}}));
        EXPECT_TRUE(siteRDFNode->setOption<RangedVector3>("DistanceRange", distanceRange));
        EXPECT_TRUE(siteRDFNode->setOption("ExcludeSameMolecule", enableSameMolecule));

        return siteRDFNode;
    }
};

TEST_F(SiteRDFNodeTest, Water)
{
    createGraph("dlpoly/water267-analysis/water-267-298K.xyz", TrajectoryImportFileFormat::TrajectoryImportFormat::XYZ);

    auto oORDF = createSiteRDFNode({"O", "O"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto h1H2RDF = createSiteRDFNode({"H1", "H2"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(h1H2RDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM", "COM"}, {{0.0, 20.0, 0.05}}, true);
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator_->setOption<Number>("N", 95));
    ASSERT_EQ(iterator_->run(), NodeConstants::ProcessResult::Success);

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
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number A", cNA, 4.32359551, 2.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number B", cNB, 19.413049, 7.0e-4));
}

TEST_F(SiteRDFNodeTest, WaterNPT)
{
    createGraph("dlpoly/water267-npt/water-267-298K.HISf", TrajectoryImportFileFormat::TrajectoryImportFormat::DLPOLYFormatted);

    auto oORDF = createSiteRDFNode({"O", "O"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto h1H2RDF = createSiteRDFNode({"H1", "H2"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(h1H2RDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM", "COM"}, {{0.0, 20.0, 0.05}}, true);
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator_->setOption<Number>("N", 95));
    ASSERT_EQ(iterator_->run(), NodeConstants::ProcessResult::Success);

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

    auto oORDF = createSiteRDFNode({"O-dyn", "O-dyn"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto hHRDF = createSiteRDFNode({"H-dyn", "H-dyn"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(hHRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator_->setOption<Number>("N", 95));
    ASSERT_EQ(iterator_->run(), NodeConstants::ProcessResult::Success);

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

    auto oORDF = createSiteRDFNode({"O-frag", "O-frag"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(oORDF->setOption("RangeBEnabled", true));

    auto hHRDF = createSiteRDFNode({"H-frag", "H-frag"}, {{0.0, 20.0, 0.01}}, true);
    ASSERT_TRUE(hHRDF->setOption("RangeBEnabled", true));

    auto comCOMRDF = createSiteRDFNode({"COM-frag", "COM-frag"}, {{0.0, 20.0, 0.05}}, true);
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeA", {0.0, 3.3}));
    ASSERT_TRUE(comCOMRDF->setOption<Range>("RangeB", {3.3, 5.6}));
    ASSERT_TRUE(comCOMRDF->setOption("RangeBEnabled", true));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator_->setOption<Number>("N", 95));
    ASSERT_EQ(iterator_->run(), NodeConstants::ProcessResult::Success);

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
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number A", cNA, 4.32359551, 2.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkSampledDouble("coordination number B", cNB, 19.413049, 7.0e-4));
}

} // namespace UnitTest