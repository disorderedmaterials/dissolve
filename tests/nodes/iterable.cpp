// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/iterableGraph.h"
#include "nodes/numberNode.h"
#include "nodes/outputs.h"
#include "nodes/registry.h"
#include "tests/testing.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class IterableGraphTest : public ::testing::Test
{
    public:
    IterableGraphTest() {}

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
        loopGraph_ = dynamic_cast<IterableGraph *>(root_.createNode("Iterator", "Iterator"));
        x_ = dynamic_cast<AddNode *>(loopGraph_->createNode("Add", "x"));
        y_ = dynamic_cast<AddNode *>(root_.createNode("Add", "y"));

        ASSERT_TRUE(i_);
        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(loopGraph_);

        ASSERT_EQ(i_->name(), "i");
        ASSERT_EQ(x_->name(), "x");
        ASSERT_EQ(y_->name(), "y");
        ASSERT_EQ(loopGraph_->name(), "Iterator");

        // Create edge connections
        // - Number 'i' is a dynamic input to the IterableGraph - we'll call the input "I"
        EXPECT_TRUE(root_.addEdge({"i", "X", "Iterator", "I"}));
        // - Add 'x' takes the IterableGraph input "I" as its parameter "X"
        EXPECT_TRUE(loopGraph_->addEdge({"Inputs", "I", "x", "X"}));
        // - Result from Add 'x' goes to graph output (which we will call "C") as well as loopback to "I"
        EXPECT_TRUE(loopGraph_->addEdge({"x", "Result", "Outputs", "C"}));
        EXPECT_TRUE(loopGraph_->addEdge({"x", "Result", "LoopBacks", "I"}));
        // - The output "C" of the loop graph then goes to input "X" of Add 'y'
        EXPECT_TRUE(root_.addEdge({"Iterator", "C", "y", "X"}));
    }

    protected:
    DissolveGraph root_;
    NumberNode *i_{nullptr};
    AddNode *x_{nullptr}, *y_{nullptr};
    IterableGraph *loopGraph_{nullptr};
};

TEST_F(IterableGraphTest, RoundTrip)
{
    createGraph();

    // Serialised graph TOML
    SerialisedValue graphTOML;
    ASSERT_NO_THROW(root_.serialise("graph", graphTOML));

    // count how many loop edges were made
    auto loop = dynamic_cast<IterableGraph *>(root_.findNode("Iterator"));
    ASSERT_TRUE(loop);
    auto correctEdges = loop->loopEdges().size();

    // Confirm that loop edges were rebuilt
    ASSERT_EQ(loop->loopEdges().size(), 1);

    // Deserialise from the stored TOML
    auto deserialisedGraph = std::make_unique<DissolveGraph>();
    ASSERT_NO_THROW(deserialisedGraph->deserialise(graphTOML["graph"]));

    // Complete round trip - re-serialise the result and compare it to the original TOML
    SerialisedValue compareTOML;
    ASSERT_NO_THROW(deserialisedGraph->serialise("graph", compareTOML));
    ASSERT_NO_THROW(UnitTest::compareToml("", graphTOML, compareTOML));

    // Confirm that loop edges were rebuilt
    loop = dynamic_cast<IterableGraph *>(deserialisedGraph->findNode("Iterator"));
    ASSERT_TRUE(loop);
    ASSERT_EQ(loop->loopEdges().size(), correctEdges);
}

TEST_F(IterableGraphTest, BasicNonLoopingSeries)
{
    auto root = std::make_unique<DissolveGraph>();
    auto loop = dynamic_cast<IterableGraph *>(root->createNode("Iterator", "Iterator"));
    auto i = dynamic_cast<NumberNode *>(root->createNode("Number", "i"));
    auto a = dynamic_cast<AddNode *>(loop->createNode("Add", "a"));
    auto b = dynamic_cast<AddNode *>(loop->createNode("Add", "b"));
    auto c = dynamic_cast<AddNode *>(loop->createNode("Add", "c"));
    ASSERT_TRUE(loop->setOption<Number>("N", 1));
    EXPECT_TRUE(root->addEdge({"i", "X", "Iterator", "I"}));

    /*
     * i, 1 -> itA, 1 + 1 = 2
     */

    // This should actually result in the sole internal node not being run
    i->setOption<Number>("X", 1);
    a->setInput<Number>("Y", 1);
    b->setInput<Number>("Y", 1);
    c->setInput<Number>("Y", 1);
    EXPECT_TRUE(loop->addEdge({"Inputs", "I", "a", "X"}));
    EXPECT_EQ(loop->run(), NodeConstants::ProcessResult::Success);
    auto res1 = a->getOutputValue<Number>("Result").asInteger();
    EXPECT_TRUE(a->versionIndex() == 0);
    EXPECT_TRUE(loop->versionIndex() == 0);
    ASSERT_EQ(res1, 2);

    // No loopbacks occur
    EXPECT_TRUE(loop->loopBacks()->versionIndex() == -1);

    /*
     * i, 1 -> itA, 1 + 1 = 2 -> itB, 1 + 2 = 3
     */
    EXPECT_TRUE(loop->addEdge({"a", "Result", "b", "X"}));
    ASSERT_TRUE(loop->setOption<Number>("N", 1));
    EXPECT_EQ(loop->run(), NodeConstants::ProcessResult::Success);
    auto res2 = b->getOutputValue<Number>("Result").asInteger();
    EXPECT_TRUE(a->versionIndex() == 1);
    EXPECT_TRUE(b->versionIndex() == 0);
    EXPECT_TRUE(loop->versionIndex() == 1);
    ASSERT_EQ(res2, 3);

    // No loopbacks occur
    EXPECT_TRUE(loop->loopBacks()->versionIndex() == -1);

    /*
     * i, 1 -> itA, 1 + 1 = 2 -> itB, 1 + 2 = 3 -> itC, 1 + 3 = 4
     */
    EXPECT_TRUE(loop->addEdge({"b", "Result", "c", "X"}));
    ASSERT_TRUE(loop->setOption<Number>("N", 1));
    EXPECT_EQ(loop->run(), NodeConstants::ProcessResult::Success);
    auto res3 = c->getOutputValue<Number>("Result").asInteger();
    EXPECT_TRUE(a->versionIndex() == 2);
    EXPECT_TRUE(b->versionIndex() == 1);
    EXPECT_TRUE(c->versionIndex() == 0);
    EXPECT_TRUE(loop->versionIndex() == 2);
    ASSERT_EQ(res3, 4);

    // No loopbacks occur
    EXPECT_TRUE(loop->loopBacks()->versionIndex() == -1);

    // Run 100 times
    ASSERT_TRUE(loop->setOption<Number>("N", 100));
    EXPECT_EQ(loop->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(a->getOutputValue<Number>("Result").asInteger(), 2);
    ASSERT_EQ(b->getOutputValue<Number>("Result").asInteger(), 3);
    ASSERT_EQ(c->getOutputValue<Number>("Result").asInteger(), 4);
    EXPECT_TRUE(a->versionIndex() == 3);
    EXPECT_TRUE(b->versionIndex() == 2);
    EXPECT_TRUE(c->versionIndex() == 1);
    EXPECT_TRUE(loop->versionIndex() == 3);

    // No loopbacks occur
    EXPECT_TRUE(loop->loopBacks()->versionIndex() == -1);

    // Upstream node change
    i->setOption<Number>("X", 2);

    /*
     * i, 2 -> itA, 1 + 2 = 3 -> itB, 1 + 3 = 4 -> itC, 1 + 4 = 5
     */

    // Run again
    ASSERT_TRUE(loop->setOption<Number>("N", 1));
    EXPECT_EQ(loop->run(), NodeConstants::ProcessResult::Success);
    ASSERT_EQ(a->getOutputValue<Number>("Result").asInteger(), 3);
    ASSERT_EQ(b->getOutputValue<Number>("Result").asInteger(), 4);
    ASSERT_EQ(c->getOutputValue<Number>("Result").asInteger(), 5);
    EXPECT_TRUE(a->versionIndex() == 4);
    EXPECT_TRUE(b->versionIndex() == 3);
    EXPECT_TRUE(c->versionIndex() == 2);
    EXPECT_TRUE(loop->versionIndex() == 4);

    // No loopbacks occur
    EXPECT_TRUE(loop->loopBacks()->versionIndex() == -1);
}

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

    auto nLoops = loopGraph_->findOption("N");
    EXPECT_TRUE(nLoops);

    nLoops->set<Number>(0);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 0);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(x_->versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), NodeConstants::InvalidVersion);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), NodeConstants::InvalidVersion);
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

    auto nLoops = loopGraph_->findOption("N");
    EXPECT_TRUE(nLoops);

    // Zero iterations: We expect 1 + (xB = 1) = 1 + 1 = 2
    nLoops->set<Number>(1);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 2);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), 0);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), NodeConstants::InvalidVersion);
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

    auto nLoops = loopGraph_->findOption("N");
    EXPECT_TRUE(nLoops);

    // One iteration: We expect (LB = 2) + (xB = 1) = 2 + 1 = 3
    nLoops->set<Number>(2);
    EXPECT_EQ(y_->run(), NodeConstants::ProcessResult::Success);
    auto res = y_->getOutputValue<Number>("Result").asInteger();
    ASSERT_EQ(res, 3);

    // Check node versioning
    EXPECT_EQ(i_->versionIndex(), 0);
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), 1);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 0
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), 0);
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

    auto nLoops = loopGraph_->findOption("N");
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
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), 9);
    EXPECT_EQ(x_->versionIndex(), 9);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), 9);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration i > 1
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), 8);
}

TEST_F(IterableGraphTest, ReleaseLoopBack)
{
    createGraph();

    const auto nEdges = loopGraph_->edges().size();

    auto flagged = loopGraph_->proxyInputs().findOutput("I");

    loopGraph_->removeEdge({"x", "Result", "LoopBacks", "I"});

    ASSERT_EQ(loopGraph_->loopEdges().size(), 0);
    ASSERT_EQ(loopGraph_->edges().size(), nEdges);
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

    auto nLoops = loopGraph_->findOption("N");
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
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), 99);
    EXPECT_EQ(x_->versionIndex(), 99);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), 99);
    EXPECT_EQ(y_->versionIndex(), 0);

    // Loopbacks node only runs on iteration 0 < i <= nLoops
    // (in 100 runs, loop backs up version 99 times, starting from -1)
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), 98);

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
    EXPECT_EQ(loopGraph_->proxyInputs().versionIndex(), 199);
    EXPECT_EQ(x_->versionIndex(), 199);
    EXPECT_EQ(loopGraph_->proxyOutputs().versionIndex(), 199);
    EXPECT_EQ(y_->versionIndex(), 1);

    // Loopbacks node only runs on iteration 0 < i <= nLoops
    // (after another 100 runs, loop backs up version a further 99 times, starting from 98)
    EXPECT_EQ(loopGraph_->loopBacks()->versionIndex(), 197);
}

} // namespace UnitTest
