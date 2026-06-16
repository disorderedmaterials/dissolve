// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(SQNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 1000}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportEPSRAtoStructure", "epsr25/water1000-neutron/waterbox.ato"));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);

    // Run the graph from the SQ node
    ASSERT_EQ(sqNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(sqNode->versionIndex(), 0);

    // Get the SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 2,
                                                1.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 4,
                                                3.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Unbound Partial", "epsr25/water1000-neutron/water.EPSR.f01", 1, 6,
                                                3.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 4,
                                                1.0e-4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 6,
                                                4.0e-5));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Bound Partial", "epsr25/water1000-neutron/water.EPSR.s01", 1, 2, 1.0e-5,
                                                Error::ErrorType::RMSEError));
}

TEST(SQNodeTest, WaterMethanol)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/water.toml", 300}, {"species/methanol.toml", 600}}, 0.1));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportEPSRAtoStructure", "epsr25/water300methanol600/watermeth.ato"));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Run the graph from the SQ node
    ASSERT_EQ(sqNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(sqNode->versionIndex(), 0);

    // Get the SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");
    ASSERT_TRUE(unweightedSQ);

    /*
     * Partial Radial Distribution Functions
     *  Order of partials in EPSR files is:
     *    2      4      6      8      10     12     14     16     18     20     22     24     26     28     30     32     34
     *  OW-OW  OW-HW  OW-CT  OW-HC  OW-OH  OW-HO  HW-HW  HW-CT  HW-HC  HW-OH  HW-HO  CT-CT  CT-HC  CT-OH  CT-HO  HC-HC  HC-OH
     *    36     38     40     42
     *  HC-HO  OH-OH  OH-HO  HO-HO
     */

    // Partial g(r) (unbound terms
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1, 2,
                                                4.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1, 4,
                                                5.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "CT")),
                                                "OW-CT Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                6));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "HC")),
                                                "OW-HC Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                8));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "OH")),
                                                "OW-OH Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                10));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OW", "HO")),
                                                "OW-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                12));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1, 14,
                                                5.0e-2));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "CT")),
                                                "HW-CT Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                16));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "HC")),
                                                "HW-HC Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                18));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "OH")),
                                                "HW-OH Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                20));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HW", "HO")),
                                                "HW-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                22));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CT", "CT")),
                                                "CT-CT Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1, 24,
                                                8.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CT", "HC")),
                                                "CT-HC Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                26));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CT", "OH")),
                                                "CT-OH Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                28));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CT", "HO")),
                                                "CT-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                30));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HC", "HC")),
                                                "HC-HC Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                32));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HC", "OH")),
                                                "HC-OH Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                34));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HC", "HO")),
                                                "HC-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                36));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OH", "OH")),
                                                "OH-OH Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                38));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("OH", "HO")),
                                                "OH-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                40));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HO", "HO")),
                                                "HO-HO Unbound Partial", "epsr25/water300methanol600/watermeth.EPSR.f01", 1,
                                                42));

    // Partial g(r) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "HW")),
                                                "OW-HW Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 4));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "HW")),
                                                "HW-HW Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 14));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CT", "HC")),
                                                "CT-HC Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 26));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CT", "OH")),
                                                "CT-OH Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 28));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CT", "HO")),
                                                "CT-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 30));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HC", "HC")),
                                                "HC-HC Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 32));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HC", "OH")),
                                                "HC-OH Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 34));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HC", "HO")),
                                                "HC-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 36));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OH", "HO")),
                                                "OH-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 40));

    // Partial g(r) (intramolecular terms, zero)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "OW")),
                                                "OW-OW Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 2,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "CT")),
                                                "OW-CT Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 6,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "HC")),
                                                "OW-HC Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 8,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "OH")),
                                                "OW-OH Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 10,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OW", "HO")),
                                                "OW-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 12,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "CT")),
                                                "HW-CT Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 16,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "HC")),
                                                "HW-HC Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 18,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "OH")),
                                                "HW-OH Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 20,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HW", "HO")),
                                                "HW-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 22,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CT", "CT")),
                                                "CT-CT Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 24,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("OH", "OH")),
                                                "OH-OH Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 38,
                                                1.0e-5, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HO", "HO")),
                                                "HO-HO Bound Partial", "epsr25/water300methanol600/watermeth.EPSR.s01", 1, 42,
                                                1.0e-5, Error::ErrorType::RMSEError));
}

TEST(SQNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    EXPECT_TRUE(testGraph.createConfiguration("Box", {{"species/benzene.toml", 200}}, 0.876,
                                              Units::DensityUnits::GramsPerCentimetreCubedUnits));
    EXPECT_TRUE(testGraph.appendSetCoordinates("ImportEPSRAtoStructure", "epsr25/benzene200-neutron/boxbenz.ato"));

    // Add correlation function nodes
    auto &&[grNode, sqNode] = testGraph.appendGRSQ(false, true);
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));
    ASSERT_TRUE(sqNode);
    ASSERT_TRUE(sqNode->setOption<WindowFunction::Form>("WindowFunction", WindowFunction::Form::Lorch0));

    // Run the graph from the SQ node
    ASSERT_EQ(sqNode->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(sqNode->versionIndex(), 0);

    // Get the SQ
    auto unweightedSQ = sqNode->getOutputValue<PartialSet *>("UnweightedSQ");

    // Partial S(Q) (unbound terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CA", "CA")),
                                                "CA-CA Unbound Partial", "epsr25/benzene200-neutron/benzene.EPSR.f01", 1, 2,
                                                7.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("CA", "HA")),
                                                "CA-HA Unbound Partial", "epsr25/benzene200-neutron/benzene.EPSR.f01", 1, 4,
                                                5.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->unboundPartials().get(DoubleKeyedMapKey("HA", "HA")),
                                                "HA-HA Unbound Partial", "epsr25/benzene200-neutron/benzene.EPSR.f01", 1, 6,
                                                8.0e-3));

    // Partial S(Q) (intramolecular terms)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CA", "CA")),
                                                "CA-CA Bound Partial", "epsr25/benzene200-neutron/benzene.EPSR.s01", 1, 2,
                                                1.1e-2, Error::ErrorType::RMSEError));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("CA", "HA")),
                                                "CA-HA Bound Partial", "epsr25/benzene200-neutron/benzene.EPSR.s01", 1, 4,
                                                5.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(unweightedSQ->boundPartials().get(DoubleKeyedMapKey("HA", "HA")),
                                                "HA-HA Bound Partial", "epsr25/benzene200-neutron/benzene.EPSR.s01", 1, 6,
                                                1.0e-2));
}

} // namespace UnitTest