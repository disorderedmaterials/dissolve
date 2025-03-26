// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/number.h"
#include "nodes/registry.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class GraphCoreTest : public ::testing::Test
{
    public:
    GraphCoreTest() : graph_(nullptr) {}

    // Create a graph for testing
    void createGraph()
    {
        /*
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
        graph_.addNode(NodeRegistry::produce("Add"), "x");
        graph_.addNode(NodeRegistry::produce("Add"), "y");
        graph_.addNode(NodeRegistry::produce("Add"), "z");

        x_ = dynamic_cast<AddNode *>(graph_.nodes()["x"].get());
        ASSERT_TRUE(x_);
        y_ = dynamic_cast<AddNode *>(graph_.nodes()["y"].get());
        ASSERT_TRUE(y_);
        z_ = dynamic_cast<AddNode *>(graph_.nodes()["z"].get());
        ASSERT_TRUE(z_);

        EXPECT_TRUE(graph_.addEdge({"x", "Result", "z", "A"}));
        EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "B"}));
    }

    protected:
    Graph graph_;
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr};
};

TEST_F(GraphCoreTest, Serialisation)
{
    createGraph();

    Graph copy(nullptr);
    auto serialised = graph_.serialise();
    std::cout << serialised << std::endl;

    EXPECT_EQ(graph_.nodes(), copy.nodes());
};

} // namespace UnitTest
