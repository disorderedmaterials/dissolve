// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/atomicMC.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/iterableGraph.h"
#include "tests/testGraph.h"
#include <vector>

namespace UnitTest
{

TEST(AtomShakeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    auto insertNode = testGraph.createConfiguration("Box", {{"species/water.toml", 1}}, 0.1);

    // Create iterable graph containing an AtomicMCNode
    ASSERT_TRUE(testGraph.appendNode("Iterator", "Iterator"));
    auto iterator = testGraph.head<IterableGraph>();
    auto atomicMCNode = dynamic_cast<AtomicMCNode *>(iterator->createNode("AtomicMC", "AtomicMC"));
    ASSERT_TRUE(atomicMCNode->setOption<Number>("ShakesPerAtom", 10));

    // Create number node to modify temperature
    ASSERT_TRUE(testGraph.appendNode("Number", "Temperature"));
    ASSERT_TRUE(testGraph.fetchHead()->setOption<Number>("X", 0));

    // Set connections
    EXPECT_TRUE(testGraph.addEdge({"Insert-Water", "Configuration", "Iterator", "Configuration"}));
    EXPECT_TRUE(iterator->addEdge({"Inputs", "Configuration", "AtomicMC", "Configuration"}));
    // TODO: Output parameter name cannot be the same as loopback (although loopback param CAN match input).
    // May need to enforce this.
    EXPECT_TRUE(iterator->addEdge({"AtomicMC", "Configuration", "Outputs", "OutputConfiguration"}));
    EXPECT_TRUE(iterator->addEdge({"AtomicMC", "Configuration", "LoopBacks", "Configuration"}));

    // Run a single iteration at 300K
    ASSERT_TRUE(iterator->setOption<Number>("N", 1));
    ASSERT_TRUE(iterator->run() == NodeConstants::ProcessResult::Success);

    // Zero K optimization for 100 iterations
    EXPECT_TRUE(testGraph.addEdge({"Temperature", "X", "Iterator", "X"}));
    EXPECT_TRUE(iterator->addEdge({"Inputs", "X", "AtomicMC", "Temperature"}));
    ASSERT_TRUE(iterator->setOption<Number>("N", 100));
    ASSERT_TRUE(iterator->run() == NodeConstants::ProcessResult::Success);

    // Check the geometry of the molecule
    auto cfg = insertNode->getOutputValue<Configuration *>("Configuration");
    auto &box = cfg->box();
    auto &atoms = cfg->atoms();
    auto rMin01 = box.minimumDistance(atoms[0].r(), atoms[1].r());
    auto rMin02 = box.minimumDistance(atoms[0].r(), atoms[2].r());
    auto angle102 = box.angleInDegrees(atoms[1].r(), atoms[0].r(), atoms[2].r());
    EXPECT_NEAR(rMin01, 1.0, 1.0e-4);
    EXPECT_NEAR(rMin02, 1.0, 1.0e-4);
    EXPECT_NEAR(angle102, 113.24, 1.7e-3);
}

} // namespace UnitTest
