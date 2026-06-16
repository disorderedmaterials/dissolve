// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/moleculeTorsion.h"
#include "nodes/iterableGraph.h"
#include "tests/graphData.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
TEST(MoleculeTorsionNodeTest, Benzene)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/benzene.toml", 181}}, {29.925089931000, 29.925089931000, 29.925089931000});

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/benzene181/benzene181.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis node to the iterator
    auto moleculeTorsion = dynamic_cast<MoleculeTorsionNode *>(iterator->createNode("MoleculeTorsion"));
    ASSERT_TRUE(moleculeTorsion);
    auto *benzene = testGraph.findNode("Benzene")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(benzene);
    ASSERT_TRUE(moleculeTorsion->setOption("Species", benzene));
    ASSERT_TRUE(moleculeTorsion->setOption<Number>("I", 0));
    ASSERT_TRUE(moleculeTorsion->setOption<Number>("J", 2));
    ASSERT_TRUE(moleculeTorsion->setOption<Number>("K", 4));
    ASSERT_TRUE(moleculeTorsion->setOption<Number>("L", 6));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "MoleculeTorsion", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 80));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    EXPECT_TRUE(DissolveSystemTest::checkData1D(moleculeTorsion->frequency(), "Normalised Frequency",
                                                "dlpoly/benzene181/benzene181.01-03-05-07.tors.norm", 1, 2, 1.0e-3));
}

} // namespace UnitTest