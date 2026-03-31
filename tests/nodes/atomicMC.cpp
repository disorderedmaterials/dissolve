// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/atomicMC/atomicMC.h"
#include "io/import/coordinates.h"
#include "io/import/data1D.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/insert.h"
#include "nodes/iterableGraph.h"
#include "nodes/numberNode.h"
#include "tests/graphData.h"
#include "tests/nodes/mermaid.h"
#include "tests/speciesData.h"
#include "tests/testData.h"
#include <array>
#include <gtest/gtest.h>
#include <map>
#include <tuple>
#include <variant>
#include <vector>

namespace UnitTest
{

TEST(AtomShakeTest, Water)
{
    // Set up the test graph
    TestGraph testGraph;
    auto insertNode = testGraph.createConfiguration("Box", {{createWater, 1}}, 0.1);

    // Create iterable graph containing an AtomicMCNode
    auto iterator = dynamic_cast<IterableGraph *>(testGraph.createNode("Iterator", "Iterator"));
    auto atomicMCNode = dynamic_cast<AtomicMCNode *>(iterator->createNode("AtomicMC", "AtomicMC"));
    ASSERT_TRUE(atomicMCNode->setOption<Number>("ShakesPerAtom", 10));

    // Create number node to modify temperature
    auto temperatureValueNode = dynamic_cast<NumberNode *>(testGraph.createNode("Number", "Temperature"));
    ASSERT_TRUE(temperatureValueNode);
    ASSERT_TRUE(temperatureValueNode->setOption<Number>("X", 0));

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
    auto *box = cfg->box();
    auto &atoms = cfg->atoms();
    auto rMin01 = box->minimumDistance(atoms[0].r(), atoms[1].r());
    auto rMin02 = box->minimumDistance(atoms[0].r(), atoms[2].r());
    auto angle102 = box->angleInDegrees(atoms[1].r(), atoms[0].r(), atoms[2].r());
    EXPECT_NEAR(rMin01, 1.0, 1.0e-4);
    EXPECT_NEAR(rMin02, 1.0, 1.0e-4);
    EXPECT_NEAR(angle102, 113.24, 1.7e-3);
    exportMermaidGraph(data.graphRoot);
}

} // namespace UnitTest