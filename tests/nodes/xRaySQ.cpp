// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/xRaySQ.h"
#include "nodes/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(XRaySQNodeTest, WaterReferenceFT)
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

    // Add XRaySQ
    auto H2Ox = testGraph.appendXRaySQ(sqNode, "H2Ox", {"epsr25/water1000-neutron-xray/PCCPfofq.txt"});
    ASSERT_TRUE(H2Ox);
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "NormaliseTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "ReferenceNormalisedTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<std::optional<Number>>("ReferenceFTQMin", 0.5));

    // Run the graph from the XRaySQ node
    ASSERT_EQ(H2Ox->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2Ox->versionIndex(), 0);

    // Total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(H2Ox->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H2Ox F(Q)",
                                                "epsr25/water1000-neutron-xray/water.EPSR.u01", 1, 8, 1.5e-4));

    // Total G(r)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(H2Ox->getOutputValue<Data1D>("ReferenceGR"), "H2Ox Reference G(r)",
                                                "epsr25/water1000-neutron-xray/water.EPSR.w01", 1, 8, 5.0e-5));
}

} // namespace UnitTest