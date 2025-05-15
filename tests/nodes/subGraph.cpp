// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
         *                             GraphA (SubGraph)
         *                             ------------------------------------------------------------------\
         *    Add (x)                  | Inputs                                                          |
         *    ------------------       |--------|                                                        |
         *   o-A = 1     result-o ---- o >> C > o -----------+                                           |
         *   o-B = 2           |       |                      \      Add (z)                  Outputs    |
         *    -----------------/       |                       \     ----------------     |--------------|
         *                             |                        +--- o-A       result-o - o >> result >> o
         *                             |  Add (y)                 +--o-B             |                   |
         *                             |  ------------------     /   ----------------/                   |
         *                             | o-A = 3     result-o --+                                        |
         *                             | o-B = 4           |                                             |
         *                             |  -----------------/                                             |
         *                             |                                                                 |
         *                             \-----------------------------------------------------------------/
         */

        // Create node X in root graph
        x_ = dynamic_cast<AddNode *>(root_.createNode("Add", "x"));
        ASSERT_TRUE(x_);
        ASSERT_EQ(x_->name(), "x");

        // Create subgraph GraphA
        graphA_ = dynamic_cast<Graph *>(root_.createNode("Graph", "GraphA"));
        ASSERT_TRUE(graphA_);
        ASSERT_EQ(graphA_->name(), "GraphA");

        // Create y and z in GraphA
        y_ = dynamic_cast<AddNode *>(graphA_->createNode("Add", "y"));
        z_ = dynamic_cast<AddNode *>(graphA_->createNode("Add", "z"));
        ASSERT_TRUE(y_);
        ASSERT_EQ(y_->name(), "y");
        ASSERT_TRUE(z_);
        ASSERT_EQ(z_->name(), "z");
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    Graph *graphA_{nullptr};
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr};
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

    // Create a dynamic input on GraphA by creating an edge to it
    EXPECT_TRUE(root_.addEdge({"x", "Result", "GraphA", "C"}));

    // Connect the dynamic input on GraphA internally to it's "z" node
    // BUT C IS NOT AN OUTPUT!!!!!
    EXPECT_TRUE(graphA_->addEdge({"Inputs", "C", "Y", "A"}));
}

} // namespace UnitTest
