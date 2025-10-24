// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/loop.h"
#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/numberNode.h"
#include "nodes/registry.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class LoopGraphTest : public ::testing::Test
{
    public:
    LoopGraphTest() : dissolve_(coreData_), root_(dissolve_) {}

    // Create a graph for testing
    void createGraph()
    {
        /*
         *    Number (i)                      LoopGraph
         *    ------------------              ----------------------------------           Add (y)
         *    |               A-o--+          |                                |-OUT-\     ------------------
         *    -----------------/    \    +-IN-|     Add (x)                +--o---C---o---o-A         result-o
         *                           \   -    |     ------------------    /    |-----/    o-B (0)           |
         *                            +-o-->>>-o---o-A         result-o--+     |           -----------------/
         *                               - |  |    o-B (1)           |    \    |-LB--\
         *                               --+--|     -----------------/     \   |     |
         *                                 |  |                             +-o-->IN----
         *                                 |  |                                |-----/  \
         *                                 \  ----------------------------------        |
         *                                  \__________________________________________/
         */
        root_.x = 0;
        root_.y = 0;

        // Create nodes
        i_ = dynamic_cast<NumberNode *>(root_.createNode("Number", "i"));
        loop_ = dynamic_cast<LoopGraph *>(root_.createNode("Loop", "loop"));
        x_ = dynamic_cast<AddNode *>(loop_->createNode("Add", "x"));
        y_ = dynamic_cast<AddNode *>(root_.createNode("Add", "y"));

        ASSERT_TRUE(i_);
        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(loop_);

        ASSERT_EQ(i_->name(), "i");
        ASSERT_EQ(x_->name(), "x");
        ASSERT_EQ(y_->name(), "y");
        ASSERT_EQ(loop_->name(), "loop");

        // Create edge connections
        // - Number 'i' is a dynamic input to the LoopGraph - we'll call the input "I"
        EXPECT_TRUE(root_.addEdge({"i", "A", "loop", "I"}));
        // - Add 'x' takes the LoopGraph input "I" as its parameter "A"
        EXPECT_TRUE(loop_->addEdge({"Inputs", "I", "x", "A"}));
        // - Result from Add 'x' goes to graph output (which we will call "C") as well as loopback to "I"
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "Outputs", "C"}));
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "Inputs", "I"}));
        // - The output "C" of the loop graph then goes to input "A" of Add 'y'
        EXPECT_TRUE(root_.addEdge({"loop", "C", "y", "A"}));
        /*
         * TODO There is an obvious, smaller unit test to write here which tests the validity of trying to make a loopback
         * connection to a named Input which doesn't exist.
         */
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    NumberNode *i_{nullptr};
    AddNode *x_{nullptr}, *y_{nullptr};
    LoopGraph *loop_{nullptr};
};

TEST_F(LoopGraphTest, BasicLoop)
{
    createGraph();

    const auto iterations = 10;

    // Set some numbers
    x_->findInput("B")->set<Number>(1);
    y_->findInput("B")->set<Number>(1);
    loop_->findOption("Iterations")->set<Number>(iterations);

    // Run y - all nodes should update
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), iterations - 1);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(i_->getOutputValue<Number>("A").asInteger(), 0);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), iterations);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), iterations + 1);

    // Change number input 'i' and run y - all nodes should update again
    i_->findOption("A")->set<Number>(5);
    EXPECT_EQ(i_->getOutputValue<Number>("A").asInteger(), 5);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 19);
    EXPECT_EQ(y_->versionIndex(), 1);
    EXPECT_EQ(i_->versionIndex(), 1);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), iterations + 5);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), iterations + 5 + 1);

    // Run y again - should be no change as no upstream data has changed
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Unchanged);
    EXPECT_EQ(x_->versionIndex(), 19);
    EXPECT_EQ(y_->versionIndex(), 1);
    EXPECT_EQ(i_->versionIndex(), 1);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), iterations + 5);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), iterations + 5 + 1);
};

} // namespace UnitTest
