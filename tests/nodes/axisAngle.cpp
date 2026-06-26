// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/axisAngle.h"
#include "nodes/species.h"
#include "tests/testGraph.h"

namespace UnitTest
{
TEST(AxisAngleNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    ASSERT_TRUE(testGraph.createConfiguration("Box", {{"species/benzene.toml", 3}}, {30.0, 30.0, 30.0}));
    ASSERT_TRUE(testGraph.appendSetCoordinates("ImportXYZStructure", "xyz/benzene-trio.xyz"));

    // Add the analysis module to the iterator
    auto axisAngle = dynamic_cast<AxisAngleNode *>(testGraph.createNode("AxisAngle"));
    ASSERT_TRUE(axisAngle);
    auto *benzene = testGraph.findNode("Benzene")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(benzene);
    ASSERT_TRUE(axisAngle->setOption<SpeciesSites>("SiteA", {{benzene->findSite("COG")}}));
    ASSERT_TRUE(axisAngle->setOption<SpeciesSites>("SiteB", {{benzene->findSite("COG")}}));
    ASSERT_TRUE(axisAngle->setOption<RangedVector3>("DistanceRange", {{0.0, 10.0, 0.1}}));
    ASSERT_TRUE(axisAngle->setOption<RangedVector3>("AngleRange", {{0.0, 90.0, 10.0}}));
    ASSERT_TRUE(axisAngle->setOption("ExcludeSameMolecule", true));
    ASSERT_TRUE(testGraph.addEdge({testGraph.fetchHeadName(), "Configuration", "AxisAngle", "Configuration"}));

    // Run from the axis angle node
    ASSERT_EQ(axisAngle->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(axisAngle->versionIndex(), 0);

    // Get the data
    auto data = axisAngle->axisAngle();
    ASSERT_EQ(data.value(4), 1.0);
    ASSERT_EQ(std::accumulate(data.values().begin(), data.values().end(), 0.0), 1.0);
}

} // namespace UnitTest