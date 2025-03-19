// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/registry.h"
#include "nodes/number.h"
#include <gtest/gtest.h>

namespace UnitTest
{

TEST(GraphFlowTest, Basic) {
        Graph graph(nullptr);

        /*
         * First, construct and test the following unconnected nodes:
         *
         *    Add (x)
         *    ------------------
         *   o-A = 1     result-o ---+
         *   o-B = 2           |      \        Add (z)
         *    -----------------/       \       ------------------
         *                              +---- o-A         result-o
         *    Add (y)                 +------ o-B               |
         *    ------------------     /         -----------------/
         *   o-A = 3     result-o --+
         *   o-B = 4           |
         *    -----------------/
         */
        // Create nodes and get references to them
        graph.addNode(NodeRegistry::produce("Add"), "x");
        graph.addNode(NodeRegistry::produce("Add"), "y");
        graph.addNode(NodeRegistry::produce("Add"), "z");
        auto &x = *dynamic_cast<AddNode *>(graph.nodes()["x"].get());
        auto xA = x.findInput("A")->upcast<Number>();
        auto xB = x.findInput("B")->upcast<Number>();
        auto xResult = x.findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(xA);
        ASSERT_TRUE(xB);
        ASSERT_TRUE(xResult);
        xA->set(1);
        xB->set(2);
        auto &y = *dynamic_cast<AddNode *>(graph.nodes()["y"].get());
        auto yA = y.findInput("A")->upcast<Number>();
        auto yB = y.findInput("B")->upcast<Number>();
        auto yResult = y.findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(yA);
        ASSERT_TRUE(yB);
        ASSERT_TRUE(yResult);
        yA->set(3);
        yB->set(4);
        auto &z = *dynamic_cast<AddNode *>(graph.nodes()["z"].get());
        auto zResult = z.findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(zResult);

        // Check nodes in isolation first - all should be able to run and give meaningful results
        EXPECT_TRUE(x.inputsAreValid());
        EXPECT_EQ(x.versionIndex(), Node::InvalidVersion);
        EXPECT_EQ(x.run(), Node::ProcessResult::Success);
        EXPECT_EQ(x.versionIndex(), 0);
        EXPECT_EQ(xResult->get().asInteger(), 3);

        EXPECT_TRUE(y.inputsAreValid());
        EXPECT_EQ(y.versionIndex(), Node::InvalidVersion);
        EXPECT_EQ(y.run(), Node::ProcessResult::Success);
        EXPECT_EQ(y.versionIndex(), 0);
        EXPECT_EQ(yResult->get().asInteger(), 7);

        EXPECT_TRUE(z.inputsAreValid());
        EXPECT_EQ(z.versionIndex(), Node::InvalidVersion);
        EXPECT_EQ(z.run(), Node::ProcessResult::Success);
        EXPECT_EQ(z.versionIndex(), 0);
        EXPECT_EQ(zResult->get().asInteger(), 0);

        // Running nodes again should not increase version index since the inputs have no dependencies
        EXPECT_EQ(x.run(), Node::ProcessResult::Success);
        EXPECT_EQ(x.versionIndex(), 0);
        EXPECT_EQ(y.run(), Node::ProcessResult::Success);
        EXPECT_EQ(y.versionIndex(), 0);
        EXPECT_EQ(z.run(), Node::ProcessResult::Success);
        EXPECT_EQ(z.versionIndex(), 0);

        /*
         * Now we'll link x's "Result" output to z's "A" input:
         *
         *    Add (x)
         *    ------------------
         *   o-A = 1     result-o ---+
         *   o-B = 2           |      \        Add (z)
         *    -----------------/       \       ------------------
         *                              +---- o-A         result-o
         *    Add (y)                         o-B               |
         *    ------------------               -----------------/
         *   o-A = 3     result-o
         *   o-B = 4           |
         *    -----------------/
         */

        // Add the edge
        EXPECT_TRUE(graph.addEdge({"x", "Result", "z", "A"}));

        // If we now run z we should use x's output without changing x itself
        EXPECT_EQ(z.run(), Node::ProcessResult::Success);
        EXPECT_EQ(z.versionIndex(), 1);
        EXPECT_EQ(zResult->get().asInteger(), 3);
        EXPECT_EQ(x.versionIndex(), 0);

        return;
        std::vector<EdgeDefinition> declaredEdges = {{"y", "Result", "",  "B"},
                                      {"x", "Result", "z", "B"},
                                      {"x", "Result", "z", "A"}};

        // Link the inputs of Z to X and Y
        EXPECT_TRUE(graph.addEdge(declaredEdges[0]));

        // System should prevent double linking a sink
        EXPECT_FALSE(graph.addEdge(declaredEdges[1]));
        EXPECT_TRUE(graph.addEdge(declaredEdges[2]));

    };

} // namespace UnitTest
