// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/xRaySQ/xRaySQ.h"
#include "nodes/gr/gr.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(XRaySQNodeTest, WaterReferenceFT)
{
    GraphTestData data;
    createWater1000Graph(&data.graphRoot, CoordinateImportFileFormat("epsr25/water1000-neutron-xray/waterbox.ato",
                                                                     CoordinateImportFileFormat::CoordinateImportFormat::EPSR));

    // Set GR options
    auto grNode = data.graphRoot.findNode("GR");
    ASSERT_TRUE(grNode);
    ASSERT_TRUE(grNode->setOption("IntraBroadening", Function1DWrapper()));
    ASSERT_TRUE(grNode->setOption<Number>("BinWidth", 0.03));

    // Set xraySQ options
    auto H2Ox = data.graphRoot.findNode("H2Ox");
    ASSERT_TRUE(H2Ox);
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "NormaliseTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<StructureFactors::NormalisationType>(
        "ReferenceNormalisedTo", StructureFactors::NormalisationType::AverageOfSquaresNormalisation));
    ASSERT_TRUE(H2Ox->setOption<std::optional<double>>("ReferenceFTQMin", 0.5));

    // Run the graph from the XRaySQ node
    ASSERT_EQ(H2Ox->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(grNode->versionIndex(), 0);
    ASSERT_EQ(H2Ox->versionIndex(), 0);

    // Total F(Q)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2Ox->getOutputValue<PartialSet *>("WeightedSQ")->total(), "H2Ox F(Q)",
        {"epsr25/water1000-neutron-xray/water.EPSR.u01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 1.5e-4));

    // Total G(r)
    EXPECT_TRUE(DissolveSystemTest::checkData1D(
        H2Ox->getOutputValue<Data1D>("ReferenceGR"), "H2Ox Reference G(r)",
        {"epsr25/water1000-neutron-xray/water.EPSR.w01", Data1DImportFileFormat::Data1DImportFormat::XY, 1, 8}, 5.0e-5));
}

} // namespace UnitTest