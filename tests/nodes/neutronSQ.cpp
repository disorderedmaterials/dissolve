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
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{createWater, 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water1000-neutron/waterbox.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Add in NeutronSQ
    auto H2O = testGraph.appendNeutronSQ(sqNode, "H2O");
    auto D2O = testGraph.appendNeutronSQ(sqNode, "D2O", {{"Water", "D2O", 1.0}});
    auto HDO =
        testGraph.appendNeutronSQ(sqNode, "5050", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}}, Exchangeables({"HW"}));

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
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{createWater, 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water1000-neutron/waterbox.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Add in NeutronSQ
    auto H2O = testGraph.appendNeutronSQ(sqNode, "H2O", {}, {},
                                         Data1DImportFileFormat("epsr25/water1000-neutron-xray/H2O.mint01",
                                                                Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    auto D2O = testGraph.appendNeutronSQ(sqNode, "D2O", {{"Water", "D2O", 1.0}}, {},
                                         Data1DImportFileFormat("epsr25/water1000-neutron-xray/D2O.mint01",
                                                                Data1DImportFileFormat::Data1DImportFormat::GudrunMint));
    auto HDO =
        testGraph.appendNeutronSQ(sqNode, "5050", {{"Water", "Natural", 1.0}, {"Water", "D2O", 1.0}}, Exchangeables({"HW"}),
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
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{createWater, 300}, {createMethanol, 600}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water300methanol600/watermeth.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Add in NeutronSQ
    std::vector<std::tuple<std::string, std::vector<std::tuple<std::string, std::string, double>>>> samples = {
        {"HHH", {{"Water", "Natural", 1.0}, {"Methanol", "Natural", 1.0}}},
        {"H5H", {{"Water", "Natural", 1.0}, {"Methanol", "Natural", 0.5}, {"Methanol", "OD-MethylH", 0.5}}},
        {"DHH", {{"Water", "D2O", 1.0}, {"Methanol", "Natural", 1.0}}},
        {"HDH", {{"Water", "Natural", 1.0}, {"Methanol", "OD-MethylH", 1.0}}},
        {"HHD", {{"Water", "Natural", 1.0}, {"Methanol", "MethylD-OH", 1.0}}},
        {"DDH", {{"Water", "D2O", 1.0}, {"Methanol", "OD-MethylH", 1.0}}},
        {"HDD", {{"Water", "Natural", 1.0}, {"Methanol", "Deuteriated", 1.0}}},
        {"DDD", {{"Water", "D2O", 1.0}, {"Methanol", "Deuteriated", 1.0}}}};
    std::map<std::string, NeutronSQNode *> neutronSQ;
    for (const auto &[name, isotopologues] : samples)
        neutronSQ[name] = testGraph.appendNeutronSQ(sqNode, name, isotopologues, Exchangeables({"HW", "HO"}));

    // Run the graph from each NeutronSQ node
    for (auto node : std::views::values(neutronSQ))
    {
        ASSERT_EQ(node->run(), NodeConstants::ProcessResult::Success);
        ASSERT_EQ(grNode->versionIndex(), 0);
        ASSERT_EQ(node->versionIndex(), 0);
    }

    // Check total F(Q)
    // u01 file: 1  2   4   6   8  10  12  14  16
    //           Q HHH H5H DHH HDH HHD DDH HDD DDD

    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["HHH"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HHH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["H5H"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H5H Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["DHH"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DHH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["HDH"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["HHD"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HHD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 5.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["DDH"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DDH Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 8.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["HDD"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "HDD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 5.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        neutronSQ["DDD"]->getOutputValue<PartialSet *>("WeightedSQ")->total(), "DDD Total F(Q)",
        {"epsr25/water300methanol600/watermeth.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 5.0e-4));
}

TEST(NeutronSQNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(
        testGraph.createConfiguration("Box", {{createBenzene, 200}}, 0.876, Units::DensityUnits::GramsPerCentimetreCubedUnits));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/benzene200-neutron/boxbenz.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Add in NeutronSQ
    auto C6H6 = testGraph.appendNeutronSQ(sqNode, "C6H6");
    auto C6D6 = testGraph.appendNeutronSQ(sqNode, "C6D6", {{"Benzene", "C6D6", 1.0}});
    auto FiftyFifty = testGraph.appendNeutronSQ(sqNode, "5050", {{"Benzene", "Natural", 1.0}, {"Benzene", "C6D6", 1.0}});

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