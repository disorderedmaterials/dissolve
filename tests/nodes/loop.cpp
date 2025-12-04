// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/loopGraph.h"
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

TEST_F(LoopGraphTest, NoFeedback)
{
    createGraph();

    auto xB = x_->findInput("B");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("B");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("A");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("NLoops");
    EXPECT_TRUE(nLoops);

    // Zero iterations: We expect 1 + (xB = 1) = 1 + 1 = 2
    nLoops->set<Number>(0);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 2);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 0);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), -1);
}

TEST_F(LoopGraphTest, SingleFeedback)
{
    createGraph();

    auto xB = x_->findInput("B");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("B");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("A");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("NLoops");
    EXPECT_TRUE(nLoops);

    // One iteration: We expect (LB = 2) + (xB = 1) = 2 + 1 = 3
    nLoops->set<Number>(1);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 3);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 1);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 0);
}

TEST_F(LoopGraphTest, ExtendedFeedback)
{
    createGraph();

    auto xB = x_->findInput("B");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("B");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("A");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("NLoops");
    EXPECT_TRUE(nLoops);

    /*
     * 10 iterations
     *
     * n         Add.     Res.
     * 0 : 1 ->  1 + 1  -> 2  -> LB
     * 1 :   ->  2 + 1  -> 3  -> LB
     * 2 :   ->  3 + 1  -> 4  -> LB
     * 3 :   ->  4 + 1  -> 5  -> LB
     * 4 :   ->  5 + 1  -> 6  -> LB
     * 5 :   ->  6 + 1  -> 7  -> LB
     * 6 :   ->  7 + 1  -> 8  -> LB
     * 7 :   ->  8 + 1  -> 9  -> LB
     * 8 :   ->  9 + 1  -> 10 -> LB
     * 9 :   -> 10 + 1  -> 11 -> LB
     * 10 :  -> 11 + 1  -> 12 -> 12
     *
     */
    nLoops->set<Number>(10);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 12);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 10);
    EXPECT_EQ(x_->versionIndex(), 10);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 10);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 9);
}

TEST_F(LoopGraphTest, ReleaseLoopBack)
{
    createGraph();

    const auto nEdges = loop_->edges().size();

    auto flagged = loop_->proxyInputs().findOutput("I");
    ASSERT_TRUE(flagged->flags().isSet(ParameterBase::ParameterFlags::LoopBack));

    loop_->removeEdge({"x", "Result", "LoopBacks", "I"});

    ASSERT_EQ(loop_->loopEdges().size(), 0);
    ASSERT_EQ(loop_->edges().size(), nEdges);
    ASSERT_FALSE(flagged->flags().isSet(ParameterBase::ParameterFlags::LoopBack));
}

TEST_F(LoopGraphTest, UpstreamChange)
{
    createGraph();

    auto xB = x_->findInput("B");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("B");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("A");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("NLoops");
    EXPECT_TRUE(nLoops);

    /*
     * 100 iterations
     *
     */
    nLoops->set<Number>(100);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 102);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 100);
    EXPECT_EQ(x_->versionIndex(), 100);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 100);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 99);

    /*
     * Alter upstream number node and run for another 100 iterations
     *
     */
    iA->set<Number>(2);

    nLoops->set<Number>(100);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res2 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res2, 103);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 1);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 201);
    EXPECT_EQ(x_->versionIndex(), 201);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 201);
    EXPECT_EQ(y_->versionIndex(), 1);

    // Loopbacks node only runs on iteration > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 199);
}

} // namespace UnitTest
