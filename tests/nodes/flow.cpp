// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/number.h"
#include "nodes/registry.h"
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

        // Create nodes and get references to them
        graph_.addNode(NodeRegistry::produce("Add"), "x");
        graph_.addNode(NodeRegistry::produce("Add"), "y");
        graph_.addNode(NodeRegistry::produce("Add"), "z");

        x_ = dynamic_cast<AddNode *>(graph_.nodes()["x"].get());
        ASSERT_TRUE(x_);
        xA_ = x_->findInput("A")->upcast<Number>();
        xB_ = x_->findInput("B")->upcast<Number>();
        xResult_ = x_->findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(xA_);
        ASSERT_TRUE(xB_);
        ASSERT_TRUE(xResult_);
        xA_->set(1);
        xB_->set(2);
        y_ = dynamic_cast<AddNode *>(graph_.nodes()["y"].get());
        ASSERT_TRUE(y_);
        yA_ = y_->findInput("A")->upcast<Number>();
        yB_ = y_->findInput("B")->upcast<Number>();
        yResult_ = y_->findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(yA_);
        ASSERT_TRUE(yB_);
        ASSERT_TRUE(yResult_);
        yA_->set(3);
        yB_->set(4);
        z_ = dynamic_cast<AddNode *>(graph_.nodes()["z"].get());
        ASSERT_TRUE(z_);
        zA_ = z_->findInput("A")->upcast<Number>();
        zB_ = z_->findInput("B")->upcast<Number>();
        zResult_ = z_->findOutput("Result")->upcast<Number>();
        ASSERT_TRUE(zA_);
        ASSERT_TRUE(zB_);
        ASSERT_TRUE(zResult_);

        if (includeEdges)
        {
            EXPECT_TRUE(graph_.addEdge({"x", "Result", "z", "A"}));
            EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "B"}));
        }
    }

    protected:
    Graph graph_;
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr};
    std::shared_ptr<Parameter<Number>> xA_{nullptr}, xB_{nullptr}, xResult_{nullptr};
    std::shared_ptr<Parameter<Number>> yA_{nullptr}, yB_{nullptr}, yResult_{nullptr};
    std::shared_ptr<Parameter<Number>> zA_{nullptr}, zB_{nullptr}, zResult_{nullptr};
};

TEST_F(GraphFlowTest, Basic)
{
    // Get the basic graph, no edges
    createGraph(false);

    // Check nodes in isolation first - all should be able to run and give meaningful results
    EXPECT_TRUE(x_->inputsAreValid());
    EXPECT_EQ(x_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(x_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(xResult_->get().asInteger(), 3);

    EXPECT_TRUE(y_->inputsAreValid());
    EXPECT_EQ(y_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(yResult_->get().asInteger(), 7);

    EXPECT_TRUE(z_->inputsAreValid());
    EXPECT_EQ(z_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(zResult_->get().asInteger(), 0);

    // Running nodes again should not increase version index since the inputs have no dependencies
    EXPECT_EQ(x_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(z_->versionIndex(), 0);

    // Add the edge between x's "Result" and z's "A" input
    EXPECT_TRUE(graph_.addEdge({"x", "Result", "z", "A"}));

    // If we now run z we should use x's output without changing x itself
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 1);
    EXPECT_EQ(zResult_->get().asInteger(), 3);
    EXPECT_EQ(x_->versionIndex(), 0);

    // Complete the graph and link y's "Result" output to z's "B" input
    EXPECT_TRUE(graph_.addEdge({"y", "Result", "z", "B"}));

    // As before, if we now run z we should use x's and y's output without changing x or y
    EXPECT_EQ(z_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(z_->versionIndex(), 2);
    EXPECT_EQ(zResult_->get().asInteger(), 10);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
};

} // namespace UnitTest
