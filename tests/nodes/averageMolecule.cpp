// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/averageMolecule.h"
#include "nodes/iterableGraph.h"
#include "nodes/species.h"
#include "tests/testing.h"

namespace UnitTest
{
TEST(AverageMoleculeNodeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/water-dlpoly.toml", 267}}, 0.1);

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "dlpoly/water267-analysis/water-267-298K.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis node to the iterator
    auto avgMol = dynamic_cast<AverageMoleculeNode *>(iterator->createNode("AverageMolecule"));
    ASSERT_TRUE(avgMol);
    auto *water = testGraph.findNode("Water")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(water);
    ASSERT_TRUE(avgMol->setOption<const SpeciesSite *>("Site", water->findSite("HMidpoint")));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "AverageMolecule", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 95));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    auto &structure = avgMol->structure();
    checkVec3(structure.atom(0)->r(), {-0.83305, 0.0, 0.0}, 1.0e-3);
    checkVec3(structure.atom(1)->r(), {0.00016, 0.60443, 0.0}, 1.0e-3);
    checkVec3(structure.atom(2)->r(), {0.83305, 0.0, 0.0}, 1.0e-3);
}

TEST(AverageMoleculeNodeTest, BeNDy)
{
    // Set up the test graph
    TestGraph testGraph;
    testGraph.createConfiguration("Box", {{"species/bendy.toml", 1}}, 1.0e-4);

    // Create trajectory iterator
    auto iterator = testGraph.appendTrajectoryIterator("ImportXYZTrajectory", "xyz/bendy-trajectory.xyz");
    EXPECT_TRUE(iterator);

    // Add the analysis node to the iterator
    auto avgMol = dynamic_cast<AverageMoleculeNode *>(iterator->createNode("AverageMolecule"));
    ASSERT_TRUE(avgMol);
    auto *bendy = testGraph.findNode("BeNDy")->getOutputValue<const Species *>("Species");
    ASSERT_TRUE(bendy);
    ASSERT_TRUE(avgMol->setOption<const SpeciesSite *>("Site", bendy->findSite("Origin")));
    ASSERT_TRUE(iterator->addEdge({testGraph.fetchHeadName(), "Configuration", "AverageMolecule", "Configuration"}));

    // Run from the iterator node explicitly
    ASSERT_TRUE(iterator->setOption<Number>("N", 201));
    ASSERT_EQ(iterator->run(), NodeConstants::ProcessResult::Success);

    auto &structure = avgMol->structure();
    checkVec3(structure.atom(0)->r(), {2.0, 0.0, 0.0}, 1.0e-3);
    checkVec3(structure.atom(1)->r(), {0.0, 0.0, 0.0}, 1.0e-3);
    checkVec3(structure.atom(2)->r(), {2.0, 2.828427, 0.0}, 1.0e-3);
}

} // namespace UnitTest