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
         *    |          Number-o--+          |                                |-OUT-\     ------------------
         *    -----------------/    \    +-IN-|     Add (x)                +--o-->>>--o---o-A         result-o
         *                           \   -    |     ------------------    /    |-----/    o-B (0)           |
         *                            +-o-->>>-o---o-A         result-o--+     |           -----------------/
         *                               -    |    o-B (1)           |    \    |-LB--\
         *                               -----|     -----------------/     \   |     |
         *                                    |                             +-o-->IN |
         *                                    |                                |-----/
         *                                    ----------------------------------
         */
        root_.x = 0;
        root_.y = 0;

        // Create nodes
        i_ = dynamic_cast<NumberNode *>(root_.createNode("Number", "i"));
        loop_ = dynamic_cast<LoopGraph *>(root_.createNode("Loop", "Loop"));
        x_ = dynamic_cast<AddNode *>(loop_->createNode("Add", "x"));
        y_ = dynamic_cast<AddNode *>(root_.createNode("Add", "y"));

        ASSERT_TRUE(i_);
        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(loop_);

        ASSERT_EQ(i_->name(), "i");
        ASSERT_EQ(x_->name(), "x");
        ASSERT_EQ(y_->name(), "y");
        ASSERT_EQ(loop_->name(), "Loop");

        // Create edge connections
        // - Number 'i' is a dynamic input to the LoopGraph - we'll call the input "I"
        EXPECT_TRUE(root_.addEdge({"i", "A", "Loop", "I"}));
        // - Add 'x' takes the LoopGraph input "I" as its parameter "A"
        EXPECT_TRUE(loop_->addEdge({"Inputs", "I", "x", "A"}));
        // - Result from Add 'x' goes to graph output (which we will call "C") as well as loopback to "I"
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "Outputs", "C"}));
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "LoopBacks", "I"}));
        // - The output "C" of the loop graph then goes to input "A" of Add 'y'
        EXPECT_TRUE(root_.addEdge({"Loop", "C", "y", "A"}));
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

TEST_F(LoopGraphTest, SimpleFeedback)
{
    createGraph();

    auto xB = x_->findInput("B");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("B");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    /*
     * Process with a single loop iteration
     */
    auto iA = i_->findOption("A");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("NLoops");
    EXPECT_TRUE(nLoops);

    // Zero iterations: We expect 1 + (xB = 1) = 1 + 1 = 2
    nLoops->set<Number>(0);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res0 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res0, 2);

    // One iteration: We expect (LB = 2) + (xB = 1) = 2 + 1 = 3
    nLoops->set<Number>(1);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res1 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res1, 3);

    // Two iterations: We expect (LB = 3) + (xB = 1) = 3 + 1 = 4
    nLoops->set<Number>(2);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res2 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res2, 4);

    // Three iterations: We expect (LB = 4) + (xB = 1) = 3 + 1 = 4
    nLoops->set<Number>(3);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res3 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res3, 5);
}

TEST_F(LoopGraphTest, BasicLoop)
{
    createGraph();

    CoreData cd;
    Dissolve d(cd);
    DissolveGraph copy(d);
    auto serialised = root_.serialise();

    SerialisedValue contents = toml::parse("dissolve/input/simple_addition_graph.toml");
    UnitTest::compareToml("", serialised, contents);

    std::cout << serialised << std::endl;
    copy.deserialise(serialised);
    auto repeat = copy.serialise();

    UnitTest::compareToml("", repeat, contents);
};

} // namespace UnitTest
