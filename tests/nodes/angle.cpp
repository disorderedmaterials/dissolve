// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/angle.h"
#include "classes/speciesSites.h"
#include "math/rangedVector3.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(AngleNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis module to the iterator
    auto angle = dynamic_cast<AngleNode *>(iterator->createNode("Angle"));
    ASSERT_TRUE(angle);
    auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);
    ASSERT_TRUE(angle->setOption<SpeciesSites>("SiteA", {{water->findSite("O")}}));
    ASSERT_TRUE(angle->setOption<SpeciesSites>("SiteB", {{water->findSite("H-dyn")}}));
    ASSERT_TRUE(angle->setOption<SpeciesSites>("SiteC", {{water->findSite("O")}}));
    ASSERT_TRUE(angle->setOption<RangedVector3>("RangeAB", {{0.9, 1.1, 0.01}}));
    ASSERT_TRUE(angle->setOption<RangedVector3>("RangeBC", {{0.0, 5.0, 0.01}}));
    ASSERT_TRUE(angle->setOption<RangedVector3>("AngleRange", {{0.0, 180.0, 1.0}}));
    ASSERT_TRUE(angle->setOption("ExcludeSameMoleculeBC", true));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "Angle", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(angle->rdfBC(), "B-C RDF",
                                                "dlpoly/water267-analysis/water-267-298K.aardf_21_23_inter_sum", 1, 2, 4.0e-3));
    EXPECT_TRUE(DissolveSystemTest::checkData1D(angle->angleABC(), "A-B-C angle",
                                                "dlpoly/water267-analysis/water-267-298K.dahist1_02_1_01_02.angle.norm", 1, 2,
                                                3.0e-6));
}

} // namespace UnitTest