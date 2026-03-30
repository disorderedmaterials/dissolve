// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(NeutronSQNodeTest, Water)
{
    // Set up the test graph
    GraphTestData data;
    auto lastNode = createConfiguration(&data.graphRoot, {{createWater, 1000}}, 0.1);
    lastNode = appendImportCoordinates(&data.graphRoot, lastNode,
                                       CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                                  CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = appendGRSQ(&data.graphRoot, lastNode, false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Add in NeutronSQ
    auto H2O = appendNeutronSQ(&data.graphRoot, sqNode, "H2O");
    auto D2O = appendNeutronSQ(&data.graphRoot, sqNode, "D2O", {{"Water", "D2O", 1.0}});
    auto HDO = appendNeutronSQ(&data.graphRoot, sqNode, "5050", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}},
                               Exchangeables({"HW"}));

    // Run the graph from each NeutronSQ node
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
    ASSERT_TRUE(HDO);
    ASSERT_EQ(HDO->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(HDO->versionIndex(), 0);

    // Check total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        D2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "D2O F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2O->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H2O F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 6.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        HDO->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDO F(Q)",
        {"epsr25/water1000-neutron/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-5));
}

TEST(NeutronSQNodeTest, WaterReferenceFT)
{
    // Set up the test graph
    GraphTestData data;
    auto lastNode = createConfiguration(&data.graphRoot, {{createWater, 1000}}, 0.1);
    lastNode = appendImportCoordinates(&data.graphRoot, lastNode,
                                       CoordinateImportFileFormat("epsr25/water1000-neutron/waterbox.ato",
                                                                  CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = appendGRSQ(&data.graphRoot, lastNode, false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Add in NeutronSQ
    auto H2O = appendNeutronSQ(&data.graphRoot, sqNode, "H2O", {}, {},
                               Data1DImportFileFormat("epsr25/water1000-neutron-xray/H2O.mint01",
                                                      Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    auto D2O = appendNeutronSQ(&data.graphRoot, sqNode, "D2O", {{"Water", "D2O", 1.0}}, {},
                               Data1DImportFileFormat("epsr25/water1000-neutron-xray/D2O.mint01",
                                                      Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    auto HDO = appendNeutronSQ(&data.graphRoot, sqNode, "5050", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}},
                               Exchangeables({"HW"}),
                               Data1DImportFileFormat("epsr25/water1000-neutron-xray/HDO.mint01",
                                                      Data1DImportFileFormat::Data1DImportFormat::GudrunMint));

    // Run the graph from each NeutronSQ node
    ASSERT_TRUE(H2O);
    ASSERT_EQ(H2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2O->versionIndex(), 0);
    ASSERT_TRUE(D2O);
    ASSERT_EQ(D2O->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(D2O->versionIndex(), 0);
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
}

TEST(NeutronSQNodeTest, Benzene)
{
    // Set up the test graph
    GraphTestData data;
    auto lastNode =
        createConfiguration(&data.graphRoot, {{createBenzene, 200}}, 0.876, Units::DensityUnits::GramsPerCentimetreCubedUnits);
    lastNode = appendImportCoordinates(&data.graphRoot, lastNode,
                                       CoordinateImportFileFormat("epsr25/benzene200-neutron/boxbenz.ato",
                                                                  CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = appendGRSQ(&data.graphRoot, lastNode, false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Add in NeutronSQ
    auto C6H6 = appendNeutronSQ(&data.graphRoot, sqNode, "C6H6");
    auto C6D6 = appendNeutronSQ(&data.graphRoot, sqNode, "C6D6", {{"Benzene", "C6D6", 1.0}});
    auto FiftyFifty = appendNeutronSQ(&data.graphRoot, sqNode, "5050", {{"Benzene", "Natural", 1.0}, {"Benzene", "C6D6", 1.0}});

    // Run the graph from each NeutronSQ node
    ASSERT_TRUE(C6H6);
    ASSERT_EQ(C6H6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(C6H6->versionIndex(), 0);
    ASSERT_TRUE(C6D6);
    ASSERT_EQ(C6D6->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(C6D6->versionIndex(), 0);
    ASSERT_TRUE(FiftyFifty);
    ASSERT_EQ(FiftyFifty->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(FiftyFifty->versionIndex(), 0);

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
}

} // namespace UnitTest