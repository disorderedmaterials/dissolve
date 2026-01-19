// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/iterableGraph.h"
#include "nodes/numberNode.h"
#include "nodes/registry.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class IterableGraphTest : public ::testing::Test
{
    public:
    IterableGraphTest() : dissolve_(coreData_), root_(dissolve_) {}

    // Create a graph for testing
    void createGraph()
    {
        /*
         *    Number (i)                      IterableGraph
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
        loop_ = dynamic_cast<IterableGraph *>(root_.createNode("Iterator", "Iterator"));
        x_ = dynamic_cast<AddNode *>(loop_->createNode("Add", "x"));
        y_ = dynamic_cast<AddNode *>(root_.createNode("Add", "y"));

        ASSERT_TRUE(i_);
        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(loop_);

        ASSERT_EQ(i_->name(), "i");
        ASSERT_EQ(x_->name(), "x");
        ASSERT_EQ(y_->name(), "y");
        ASSERT_EQ(loop_->name(), "Iterator");

        // Create edge connections
        // - Number 'i' is a dynamic input to the IterableGraph - we'll call the input "I"
        EXPECT_TRUE(root_.addEdge({"i", "X", "Iterator", "I"}));
        // - Add 'x' takes the IterableGraph input "I" as its parameter "X"
        EXPECT_TRUE(loop_->addEdge({"Inputs", "I", "x", "X"}));
        // - Result from Add 'x' goes to graph output (which we will call "C") as well as loopback to "I"
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "Outputs", "C"}));
        EXPECT_TRUE(loop_->addEdge({"x", "Result", "LoopBacks", "I"}));
        // - The output "C" of the loop graph then goes to input "X" of Add 'y'
        EXPECT_TRUE(root_.addEdge({"Iterator", "C", "y", "X"}));
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    NumberNode *i_{nullptr};
    AddNode *x_{nullptr}, *y_{nullptr};
    IterableGraph *loop_{nullptr};
};

TEST_F(IterableGraphTest, NoRun)
{
    createGraph();

    auto xB = x_->findInput("Y");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("Y");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("X");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("N");
    EXPECT_TRUE(nLoops);

    nLoops->set<Number>(0);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 0);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(x_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), NodeConstants::InvalidVersion);
}

TEST_F(IterableGraphTest, NoFeedback)
{
    createGraph();

    auto xB = x_->findInput("Y");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("Y");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("X");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("N");
    EXPECT_TRUE(nLoops);

    // Zero iterations: We expect 1 + (xB = 1) = 1 + 1 = 2
    nLoops->set<Number>(1);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 2);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 0);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), NodeConstants::InvalidVersion);
}

TEST_F(IterableGraphTest, SingleFeedback)
{
    createGraph();

    auto xB = x_->findInput("Y");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("Y");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("X");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("N");
    EXPECT_TRUE(nLoops);

    // One iteration: We expect (LB = 2) + (xB = 1) = 2 + 1 = 3
    nLoops->set<Number>(2);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 3);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 1);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 0);
}

TEST_F(IterableGraphTest, ExtendedFeedback)
{
    createGraph();

    auto xB = x_->findInput("Y");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("Y");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("X");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("N");
    EXPECT_TRUE(nLoops);

    /*
     * 10 iterations
     *
     * i         Add.     Res.
     *
     * 1 : 1 ->  1 + 1  -> 2  -> LB
     * 2 :   ->  2 + 1  -> 3  -> LB
     * 3 :   ->  3 + 1  -> 4  -> LB
     * 4 :   ->  4 + 1  -> 5  -> LB
     * 5 :   ->  5 + 1  -> 6  -> LB
     * 6 :   ->  6 + 1  -> 7  -> LB
     * 7 :   ->  7 + 1  -> 8  -> LB
     * 8 :   ->  8 + 1  -> 9  -> LB
     * 9 :   ->  9 + 1  -> 10 -> LB
     * 10 :  -> 10 + 1  -> 11 -> LB
     *
     */
    nLoops->set<Number>(10);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 11);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 9);
    EXPECT_EQ(x_->versionIndex(), 9);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 9);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 1
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 8);
}

TEST_F(IterableGraphTest, ReleaseLoopBack)
{
    createGraph();

    const auto nEdges = loop_->edges().size();

    auto flagged = loop_->proxyInputs().findOutput("I");

    loop_->removeEdge({"x", "Result", "LoopBacks", "I"});

    ASSERT_EQ(loop_->loopEdges().size(), 0);
    ASSERT_EQ(loop_->edges().size(), nEdges);
}

TEST_F(IterableGraphTest, UpstreamChange)
{
    createGraph();

    auto xB = x_->findInput("Y");
    EXPECT_TRUE(xB);
    xB->set<Number>(1);

    auto yB = y_->findInput("Y");
    EXPECT_TRUE(yB);
    yB->set<Number>(0);

    auto iA = i_->findOption("X");
    EXPECT_TRUE(iA);
    iA->set<Number>(1);

    auto nLoops = loop_->findOption("N");
    EXPECT_TRUE(nLoops);

    /*
     * 100 iterations
     *
     */
    const int loopFor = 100;

    nLoops->set<Number>(loopFor);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 101);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 99);
    EXPECT_EQ(x_->versionIndex(), 99);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 99);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration 0 < i <= nLoops
    // (in 100 runs, loop backs up version 99 times, starting from -1)
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 98);

    /*
     * Alter upstream number node and run for another 100 iterations
     *
     */
    iA->set<Number>(2);

    nLoops->set<Number>(loopFor);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res2 = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res2, 102);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 1);
    EXPECT_EQ(loop_->proxyInputs().versionIndex(), 199);
    EXPECT_EQ(x_->versionIndex(), 199);
    EXPECT_EQ(loop_->proxyOutputs().versionIndex(), 199);
    EXPECT_EQ(y_->versionIndex(), 1);

    // Loopbacks node only runs on iteration 0 < i <= nLoops
    // (after another 100 runs, loop backs up version a further 99 times, starting from 98)
    EXPECT_EQ(loop_->loopBacks()->versionIndex(), 197);
}

} // namespace UnitTest
