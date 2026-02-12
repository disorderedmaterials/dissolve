// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/number.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class SubGraphTest : public ::testing::Test
{
    public:
    SubGraphTest() : dissolve_(coreData_), root_(dissolve_) {}

    // Create a graph for testing (no edges)
    void createGraph()
    {
        /*
         *                          GraphA (SubGraph)
         *                          --------------------------------------------------------------\
         *   Add (x)                | Inputs                                                      |
         *   ------------------     |--------|                                                    |
         *  o-A = 1     result-o -- o--> C >--o ----------+                                       |
         *  o-B = 2           |     |--------/             \       Add (z)                Outputs |      Add (w)
         *   -----------------/     |                       \      ----------------     |---------|      -----------------
         *                          |                        +--- o-A       result-o - o---> D >---o -- o-A        result-o
         *                          |  Add (y)                 +--o-B             |     ----------|     o-B = 5          |
         *                          |  ------------------     /    ---------------/               |      ----------------/
         *                          | o-A = 3     result-o --+                                    |
         *                          | o-B = 4           |                                         |
         *                          |  -----------------/                                         |
         *                          |                                                             |
         *                          \-------------------------------------------------------------/
         */

        // Create node X in root graph
        x_ = dynamic_cast<AddNode *>(root_.createNode("Add", "x"));
        ASSERT_TRUE(x_);
        ASSERT_EQ(x_->name(), "x");
        xA_ = x_->findInput("X");
        xB_ = x_->findInput("Y");
        ASSERT_TRUE(xA_);
        ASSERT_TRUE(xB_);
        xA_->set(Number{1});
        xB_->set(Number{2});

        // Create subgraph GraphA
        graphA_ = dynamic_cast<Graph *>(root_.createNode("Graph", "GraphA"));
        ASSERT_TRUE(graphA_);
        ASSERT_EQ(graphA_->name(), "GraphA");

        // Create y and z in GraphA
        y_ = dynamic_cast<AddNode *>(graphA_->createNode("Add", "y"));
        ASSERT_TRUE(y_);
        ASSERT_EQ(y_->name(), "y");
        yA_ = y_->findInput("X");
        yB_ = y_->findInput("Y");
        ASSERT_TRUE(yA_);
        ASSERT_TRUE(yB_);
        yA_->set(Number{3});
        yB_->set(Number{4});
        z_ = dynamic_cast<AddNode *>(graphA_->createNode("Add", "z"));
        ASSERT_TRUE(z_);
        ASSERT_EQ(z_->name(), "z");

        // Create w in root graph
        w_ = dynamic_cast<AddNode *>(root_.createNode("Add", "w"));
        ASSERT_TRUE(w_);
        ASSERT_EQ(w_->name(), "w");
        wB_ = w_->findInput("Y");
        ASSERT_TRUE(wB_);
        wB_->set(Number{5});
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    Graph *graphA_{nullptr};
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr}, *w_{nullptr};
    std::shared_ptr<ParameterBase> xA_{nullptr}, xB_{nullptr};
    std::shared_ptr<ParameterBase> yA_{nullptr}, yB_{nullptr};
    std::shared_ptr<ParameterBase> wB_{nullptr};
};

TEST_F(SubGraphTest, Serialisation){
    //    createGraph();
    //
    //    CoreData cd;
    //    Dissolve d(cd);
    //    DissolveGraph copy(d);
    //    auto serialised = root_.serialise();
    //
    //    SerialisedValue contents = toml::parse("dissolve/input/simple_addition_graph.toml");
    //    UnitTest::compareToml("", serialised, contents);
    //
    //    std::cout << serialised << std::endl;
    //    copy.deserialise(serialised);
    //    auto repeat = copy.serialise();
    //
    //    UnitTest::compareToml("", repeat, contents);
};

TEST_F(SubGraphTest, Connections)
{
    createGraph();

    // Create a mapped input on GraphA by creating an edge to it
    EXPECT_TRUE(root_.addEdge({"x", "Result", "GraphA", "C"}));

    // Connect the mapped input on GraphA internally to it's "z" node
    EXPECT_TRUE(graphA_->addEdge({"Inputs", "C", "z", "X"}));

    // Connect y result to z
    EXPECT_TRUE(graphA_->addEdge({"y", "Result", "z", "Y"}));

    // Connect z result to graphA output, creating a mapped output
    EXPECT_TRUE(graphA_->addEdge({"z", "Result", "Outputs", "D"}));

    // Connect GraphA mapped output "D" to node "w"
    EXPECT_TRUE(root_.addEdge({"GraphA", "D", "w", "X"}));
}

TEST_F(SubGraphTest, Flow)
{
    createGraph();

    // Create a mapped input on GraphA by creating an edge to it
    EXPECT_TRUE(root_.addEdge({"x", "Result", "GraphA", "C"}));

    // Connect the mapped input on GraphA internally to it's "z" node
    EXPECT_TRUE(graphA_->addEdge({"Inputs", "C", "z", "X"}));

    // Connect y result to z
    EXPECT_TRUE(graphA_->addEdge({"y", "Result", "z", "Y"}));

    // Connect z result to graphA output, creating a mapped output
    EXPECT_TRUE(graphA_->addEdge({"z", "Result", "Outputs", "D"}));

    // Connect GraphA mapped output "D" to node "w"
    EXPECT_TRUE(root_.addEdge({"GraphA", "D", "w", "X"}));

    // Run w - all nodes should update
    EXPECT_EQ(w_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 0);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), 0);
    EXPECT_EQ(w_->versionIndex(), 0);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), 3);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), 7);
    EXPECT_EQ(z_->getOutputValue<Number>("Result").asInteger(), 10);
    EXPECT_EQ(w_->getOutputValue<Number>("Result").asInteger(), 15);

    // Change value in 'x' and run w again - all nodes should update
    xA_->set(Number{3});
    EXPECT_EQ(w_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 0);
    EXPECT_EQ(z_->versionIndex(), 1);
    EXPECT_EQ(w_->versionIndex(), 1);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), 5);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), 7);
    EXPECT_EQ(z_->getOutputValue<Number>("Result").asInteger(), 12);
    EXPECT_EQ(w_->getOutputValue<Number>("Result").asInteger(), 17);

    // Change value in 'y' and run w again - y, z, and w should update
    yA_->set(Number{5});
    EXPECT_EQ(w_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 1);
    EXPECT_EQ(z_->versionIndex(), 2);
    EXPECT_EQ(w_->versionIndex(), 2);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), 5);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), 9);
    EXPECT_EQ(z_->getOutputValue<Number>("Result").asInteger(), 14);
    EXPECT_EQ(w_->getOutputValue<Number>("Result").asInteger(), 19);

    // Change value in 'w' and run w again - only w should update
    wB_->set(Number{7});
    EXPECT_EQ(w_->run(), NodeConstants::ProcessResult::Success);
    EXPECT_EQ(x_->versionIndex(), 1);
    EXPECT_EQ(y_->versionIndex(), 1);
    EXPECT_EQ(z_->versionIndex(), 2);
    EXPECT_EQ(w_->versionIndex(), 3);
    EXPECT_EQ(x_->getOutputValue<Number>("Result").asInteger(), 5);
    EXPECT_EQ(y_->getOutputValue<Number>("Result").asInteger(), 9);
    EXPECT_EQ(z_->getOutputValue<Number>("Result").asInteger(), 14);
    EXPECT_EQ(w_->getOutputValue<Number>("Result").asInteger(), 21);
}

} // namespace UnitTest
