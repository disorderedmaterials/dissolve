// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/gr/gr.h"
#include "math/windowFunction.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(GRNodeTest, Methods)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{[] { return createAtomic(Elements::Ar); }, 5000}}, 0.0213));

    // Append GR and SQ nodes
    auto sqNode = testGraph.appendGRSQ(true, true);
    auto arSpeciesNode = testGraph.findNode("Ar");
    ASSERT_TRUE(arSpeciesNode);
    auto grNode = testGraph.findNode("GR");

    // Calculate baseline GR with the "Test" method, a simple, serial double-loop
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::TestMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    auto rawGRBaseline = *grNode->getOutputValue<PartialSet *>("RawGR");

    // Test against simple method
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::SimpleMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    auto rawGRSimple = *grNode->getOutputValue<PartialSet *>("RawGR");
    ASSERT_EQ(grNode->versionIndex(), 1);
    ASSERT_TRUE(DissolveSystemTest::checkPartialSet(rawGRBaseline, rawGRSimple, 1.0e-8));

    // Test against cells method
    ASSERT_TRUE(grNode->setOption<GRNode::PartialsMethod>("Method", GRNode::PartialsMethod::CellsMethod));
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    auto rawGRCells = *grNode->getOutputValue<PartialSet *>("RawGR");
    ASSERT_EQ(grNode->versionIndex(), 2);
    ASSERT_TRUE(DissolveSystemTest::checkPartialSet(rawGRBaseline, rawGRCells, 1.0e-8));
}

TEST(GRNodeTest, Water)
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

    // Run the graph from the GR node
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);

    // Get the raw GR
    auto rawGR = grNode->getOutputValue<PartialSet *>("RawGR");

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 6.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "OW")), "HW-OW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Unbound Partial",
        {"epsr25/water1000-neutron/water.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 2.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "OW")), "HW-OW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Bound Partial",
        {"epsr25/water1000-neutron/water.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.5e-2));
}

TEST(GRNodeTest, WaterMethanol)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{createWater, 300}, {createMethanol, 600}}, 0.1));
    EXPECT_TRUE(testGraph.appendImportCoordinates(CoordinateImportFileFormat(
        "epsr25/water300methanol600/watermeth.ato", CoordinateImportFileFormat::CoordinateImportFormat::EPSR)));

    // Add correlation function nodes
    auto &&[grNode, _] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Run the graph from the GR node
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);

    // Get the raw GR
    auto rawGR = grNode->getOutputValue<PartialSet *>("RawGR");

    /*
     * Partial Radial Distribution Functions
     *  Order of partials in EPSR files is:
     *    2      4      6      8      10     12     14     16     18     20     22     24     26     28     30     32     34
     *  OW-OW  OW-HW  OW-CT  OW-HC  OW-OH  OW-HO  HW-HW  HW-CT  HW-HC  HW-OH  HW-HO  CT-CT  CT-HC  CT-OH  CT-HO  HC-HC  HC-OH
     *    36     38     40     42
     *  HC-HO  OH-OH  OH-HO  HO-HO
     */

    // Partial g(r) (unbound terms
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "HW")), "OW-HW Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "CT")), "OW-CT Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "HC")), "OW-HC Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 7.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "OH")), "OW-OH Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OW", "HO")), "OW-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 0.3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 0.4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "CT")), "HW-CT Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HC")), "HW-HC Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}, 4.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "OH")), "HW-OH Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HW", "HO")), "HW-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CT", "CT")), "CT-CT Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CT", "HC")), "CT-HC Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}, 4.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CT", "OH")), "CT-OH Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CT", "HO")), "CT-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HC", "HC")), "HC-HC Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}, 4.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HC", "OH")), "HC-OH Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}, 4.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HC", "HO")), "HC-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}, 5.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OH", "OH")), "OH-OH Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}, 0.3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("OH", "HO")), "OH-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}, 0.1));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HO", "HO")), "HO-HO Unbound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}, 0.3));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "HW")), "OW-HW Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.8));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HW")), "HW-HW Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 14}, 0.5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CT", "HC")), "CT-HC Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 26}, 0.3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CT", "OH")), "CT-OH Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 28}, 0.5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CT", "HO")), "CT-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 30}, 0.2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HC", "HC")), "HC-HC Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 32}, 0.06));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HC", "OH")), "HC-OH Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 34}, 0.08));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HC", "HO")), "HC-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 36}, 0.5));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OH", "HO")), "OH-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 40}, 0.5));

    // Partial g(r) (intramolecular terms, zero)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "OW")), "OW-OW Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "CT")), "OW-CT Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "HC")), "OW-HC Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "OH")), "OW-OH Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 10}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OW", "HO")), "OW-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 12}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "CT")), "HW-CT Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 16}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HC")), "HW-HC Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 18}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "OH")), "HW-OH Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 20}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HW", "HO")), "HW-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 22}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CT", "CT")), "CT-CT Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 24}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("OH", "OH")), "OH-OH Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 38}, 1.0e-5,
        Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HO", "HO")), "HO-HO Bound Partial",
        {"epsr25/water300methanol600/watermeth.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 42}, 1.0e-5,
        Error::ErrorType::RMSEError));
}

TEST(GRNodeTest, Benzene)
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

    // Run the graph from the GR node
    ASSERT_EQ(grNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);

    // Get the raw GR
    auto rawGR = grNode->getOutputValue<PartialSet *>("RawGR");

    // Partial g(r) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CA", "CA")), "CA-CA Unbound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 3.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("CA", "HA")), "CA-HA Unbound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 2.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->unboundPartials().get(DoubleKeyedMapKey("HA", "HA")), "HA-HA Unbound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.g01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 4.0e-2));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CA", "CA")), "CA-CA Bound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 2}, 0.12));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("CA", "HA")), "CA-HA Bound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 4}, 0.18));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        rawGR->boundPartials().get(DoubleKeyedMapKey("HA", "HA")), "HA-HA Bound Partial",
        {"epsr25/benzene200-neutron/benzene.EPSR.y01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 6}, 9.0e-2));
}

} // namespace UnitTest