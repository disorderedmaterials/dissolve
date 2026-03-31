// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/nodes/mermaid.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(NeutronSQNodeTest, Water)
{
    GraphTestData data;
    createWaterGraph(&data.graphRoot, 1000,
                     CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Set GR options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Run the graph from each NeutronSQ node
    auto H2O = data.graphRoot.findNode("H2O");
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    auto D2O = data.graphRoot.findNode("D2O");
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
    auto HDO = data.graphRoot.findNode("HDO");
    ASSERT_TRUE(HDO);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDO->versionIndex(), 0);

    // Check total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        D2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDO->getOutputValue<PartialSet *>("WeightedSQ")->total(), "Total F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-5));
    exportMermaidGraph(data.graphRoot);
}

TEST(NeutronSQNodeTest, WaterReferenceFT)
{
    GraphTestData data;
    createWaterGraph(&data.graphRoot, 1000,
                     CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Set GR options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Run the graph from each NeutronSQ and XRaySQ node
    auto H2O = data.graphRoot.findNode("H2O");
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    auto D2O = data.graphRoot.findNode("D2O");
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
    auto HDO = data.graphRoot.findNode("HDO");
    ASSERT_TRUE(HDO);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDO->versionIndex(), 0);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        D2O->getOutputValue<Data1D>("ReferenceGR"), "D2O Reference G(r)",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 5.0e-5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2O->getOutputValue<Data1D>("ReferenceGR"), "H2O Reference G(r)",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 5.0e-5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDO->getOutputValue<Data1D>("ReferenceGR"), "HDO Reference G(r)",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 5.0e-5));
    exportMermaidGraph(data.graphRoot);
}

TEST(NeutronSQNodeTest, WaterMethanol)
{
    GraphTestData data;
    createWaterMethanolGraph(&data.graphRoot);

    // Set GR and SQ options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    auto sqNode = data.graphRoot.findNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Run the graph from each NeutronSQ node
    auto HHH = data.graphRoot.findNode("HHH");
    ASSERT_TRUE(HHH);
    ASSERT_EQ(HHH->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HHH->versionIndex(), 0);
    auto H5H = data.graphRoot.findNode("H5H");
    ASSERT_TRUE(H5H);
    ASSERT_EQ(H5H->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H5H->versionIndex(), 0);
    auto DHH = data.graphRoot.findNode("DHH");
    ASSERT_TRUE(DHH);
    ASSERT_EQ(DHH->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(DHH->versionIndex(), 0);
    auto HDH = data.graphRoot.findNode("HDH");
    ASSERT_TRUE(HDH);
    ASSERT_EQ(HDH->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDH->versionIndex(), 0);
    auto HHD = data.graphRoot.findNode("HHD");
    ASSERT_TRUE(HHD);
    ASSERT_EQ(HHD->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HHD->versionIndex(), 0);
    auto DDH = data.graphRoot.findNode("DDH");
    ASSERT_TRUE(DDH);
    ASSERT_EQ(DDH->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(DDH->versionIndex(), 0);
    auto HDD = data.graphRoot.findNode("HDD");
    ASSERT_TRUE(HDD);
    ASSERT_EQ(HDD->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDD->versionIndex(), 0);
    auto DDD = data.graphRoot.findNode("DDD");
    ASSERT_TRUE(DDD);
    ASSERT_EQ(DDD->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(DDD->versionIndex(), 0);

    // Check total F(Q)
    // u01 file: 1  2   4   6   8  10  12  14  16
    //           Q HHH H5H DHH HDH HHD DDH HDD DDD

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HHH->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HHH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H5H->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H5H Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        DHH->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DHH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDH->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HHD->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HHD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 5.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        DDH->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DDH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 8.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDD->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 5.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        DDD->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DDD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 5.0e-4));
    exportMermaidGraph(data.graphRoot);
}

TEST(NeutronSQNodeTest, Benzene)
{
    GraphTestData data;
    createBenzeneGraph(&data.graphRoot);

    // Set GR and SQ options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    auto sqNode = data.graphRoot.findNode("SQ");
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Run the graph from each NeutronSQ node
    auto C6H6 = data.graphRoot.findNode("C6H6");
    ASSERT_TRUE(C6H6);
    ASSERT_EQ(C6H6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(C6H6->versionIndex(), 0);
    auto C6D6 = data.graphRoot.findNode("C6D6");
    ASSERT_TRUE(C6D6);
    ASSERT_EQ(C6D6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(C6D6->versionIndex(), 0);
    auto FiftyFifty = data.graphRoot.findNode("5050");
    ASSERT_TRUE(FiftyFifty);
    ASSERT_EQ(FiftyFifty->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(FiftyFifty->versionIndex(), 0);

    // Get the SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    // Total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        C6H6->getOutputValue<PartialSet *>("WeightedSQ")->total(), "C6H6 Total F(Q)",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 2.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        C6D6->getOutputValue<PartialSet *>("WeightedSQ")->total(), "C6D6 Total F(Q)",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        FiftyFifty->getOutputValue<PartialSet *>("WeightedSQ")->total(), "5050 Total F(Q)",
        {"epsr25/benzene200-neutron/benzene.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-3));
    exportMermaidGraph(data.graphRoot);
}

} // namespace UnitTest
