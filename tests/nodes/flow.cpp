// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include "nodes/number.h"
#include "nodes/numberNode.h"
#include "tests/nodes/mermaid.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class GraphFlowTest : public ::testing::Test
{
    public:
    GraphFlowTest() : graph_(nullptr) {}

    // Create a graph for testing
    void createGraph(bool includeEdges)
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

        // Create nodes
        auto *x = graph_.createNode("Add", "x");
        x_ = dynamic_cast<AddNode *>(x);
        y_ = dynamic_cast<AddNode *>(graph_.createNode("Add", "y"));
        z_ = dynamic_cast<AddNode *>(graph_.createNode("Add", "z"));

        ASSERT_TRUE(x_);
        xA_ = x_->findInput("X");
        xB_ = x_->findInput("Y");
        xResult_ = x_->findOutput("Result");
        ASSERT_TRUE(xA_);
        ASSERT_TRUE(xB_);
        ASSERT_TRUE(xResult_);
        xA_->set(Number{1});
        xB_->set(Number{2});

        ASSERT_TRUE(y_);
        yA_ = y_->findInput("X");
        yB_ = y_->findInput("Y");
        yResult_ = y_->findOutput("Result");
        ASSERT_TRUE(yA_);
        ASSERT_TRUE(yB_);
        ASSERT_TRUE(yResult_);
        yA_->set(Number{3});
        yB_->set(Number{4});

        ASSERT_TRUE(z_);
        zA_ = z_->findInput("X");
        zB_ = z_->findInput("Y");
        zResult_ = z_->findOutput("Result");
        ASSERT_TRUE(zA_);
        ASSERT_TRUE(zB_);
        ASSERT_TRUE(zResult_);

        if (includeEdges)
        {
            EXPECT_TRUE(graph_.addEdge({"x", "Result", "z", "X"}));
            EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "Y"}));
        }
    }

    protected:
    Graph graph_;
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr};
    std::shared_ptr<ParameterBase> xA_{nullptr}, xB_{nullptr}, xResult_{nullptr};
    std::shared_ptr<ParameterBase> yA_{nullptr}, yB_{nullptr}, yResult_{nullptr};
    std::shared_ptr<ParameterBase> zA_{nullptr}, zB_{nullptr}, zResult_{nullptr};
};

TEST_F(GraphFlowTest, Basic)
{
    // Get the basic graph, no edges
    createGraph(false);

    // Check nodes in isolation first - all should be able to run and give meaningful results
    EXPECT_TRUE(x_->inputsAreValid());
    EXPECT_FALSE(x_->isUpToDate());
    EXPECT_EQ(x_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(x_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(xResult_->get<Number>().asInteger(), 3);
    EXPECT_TRUE(x_->isUpToDate());

    EXPECT_TRUE(y_->inputsAreValid());
    EXPECT_FALSE(y_->isUpToDate());
    EXPECT_EQ(y_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(yResult_->get<Number>().asInteger(), 7);
    EXPECT_TRUE(y_->isUpToDate());

    EXPECT_TRUE(z_->inputsAreValid());
    EXPECT_FALSE(z_->isUpToDate());
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 0);
    EXPECT_TRUE(z_->isUpToDate());

    // Running nodes again should not increase version index since the inputs have no dependencies
    EXPECT_EQ(x_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_TRUE(x_->isUpToDate());
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_TRUE(y_->isUpToDate());
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_TRUE(z_->isUpToDate());

    // Add the edge between x's "Result" and z's "X" input
    EXPECT_TRUE(graph_.addEdge({"x", "Result", "z", "X"}));
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);

    // If we now run z we should use x's output without changing x itself
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 3);
    EXPECT_EQ(x_->versionIndex(), 0);

    // Complete the graph and link y's "Result" output to z's "Y" input
    EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "Y"}));
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);

    // As before, if we now run z we should use x's and y's output without changing x or y
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 10);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
};

TEST_F(GraphFlowTest, SetInput)
{
    // Get the basic graph
    createGraph(true);

    // Run z - all nodes should update
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 10);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Set the input A of 'x' manually. This should invalidate 'x' alone.
    xA_->set(Number{0});
    EXPECT_FALSE(x_->isUpToDate());
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), 0);

    // Run z again - it should be forced to reprocess and update itself, along with 'x' - 'y' remains unchanged
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), 1);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 9);

    // One more time
    xB_->set(Number{10});
    EXPECT_FALSE(x_->isUpToDate());
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 2);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), 2);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 17);

    // And now for y
    yB_->set(Number{5});
    EXPECT_TRUE(x_->isUpToDate());
    EXPECT_FALSE(y_->isUpToDate());
    EXPECT_FALSE(z_->isUpToDate());
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 2);
    EXPECT_EQ(y_->versionIndex(), 1);
    EXPECT_EQ(z_->versionIndex(), 3);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 18);

    EXPECT_EQ(z_->ancestors<AddNode>().size(), 3);
    EXPECT_EQ(z_->ancestors<NumberNode>().size(), 0);
}

TEST_F(GraphFlowTest, RemoveEdges)
{
    // Get the basic graph
    createGraph(true);

    // Run z - all nodes should update
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get<Number>().asInteger(), 10);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Remove edge between x and z - this will invalidate z but not x
    EXPECT_TRUE(graph_.removeEdge({"x", "Result", "z", "X"}));
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);

    // Now remove edge between y and z - this will invalidate z but not y
    EXPECT_TRUE(graph_.removeEdge({"y", "Result", "z", "Y"}));
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);

    // Try to remove a non-existent edge
    EXPECT_FALSE(graph_.removeEdge({"Q", "Result", "z", "C"}));

    // Insert a new node after edge removal
    auto x2 = dynamic_cast<AddNode *>(graph_.createNode("Add", "x2"));
    EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "Y"}));
    EXPECT_TRUE(graph_.addEdge({"x", "Result", "x2", "X"}));
    EXPECT_TRUE(graph_.addEdge({"x2", "Result", "z", "X"}));
    auto x2a = x2->findInput("Y");
    x2a->set(Number{20});
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->findOutput("Result")->get<Number>().asInteger(), 30);
    exportMermaidGraph(graph_);
}

} // namespace UnitTest
