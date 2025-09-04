// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/registry.h"
#include "tests/testData.h"
#include <gtest/gtest.h>

namespace UnitTest
{
class GraphCoreTest : public ::testing::Test
{
    public:
    GraphCoreTest() : dissolve_(coreData_), root_(dissolve_) {}

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
        root_.x = 0;
        root_.y = 0;

        // Create nodes
        x_ = dynamic_cast<AddNode *>(root_.createNode("Add", "x"));
        x_->x = 300;
        x_->y = 400;
        y_ = dynamic_cast<AddNode *>(root_.createNode("Add", "y"));
        y_->x = 350;
        y_->y = 600;
        z_ = dynamic_cast<AddNode *>(root_.createNode("Add", "z"));
        z_->x = 750;
        z_->y = 500;

        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(z_);

        ASSERT_EQ(x_->name(), "x");
        ASSERT_EQ(y_->name(), "y");
        ASSERT_EQ(z_->name(), "z");

        EXPECT_TRUE(root_.addEdge({"x", "Result", "z", "A"}));
        EXPECT_TRUE(root_.addEdge({"y", "Result", "z", "B"}));
    }

    protected:
    // We need a CoreData and Dissolve definition to properly instantiate DissolveGraph at present.
    CoreData coreData_;
    Dissolve dissolve_;
    DissolveGraph root_;
    AddNode *x_{nullptr}, *y_{nullptr}, *z_{nullptr};
};

TEST_F(GraphCoreTest, Serialisation)
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

TEST_F(GraphCoreTest, UniqueNaming)
{
    createGraph();

    // Add nodes with duplicate name
    EXPECT_EQ(root_.createNode("Add", "x")->name(), "x01");
    EXPECT_EQ(root_.createNode("Add", "x")->name(), "x02");

    // Rename existing node
    y_->setName("x");
    EXPECT_EQ(y_->name(), "x03");
    y_->setName("y");
    EXPECT_EQ(y_->name(), "y");

    // Rename existing node to same name
    z_->setName("z");
    EXPECT_EQ(z_->name(), "z");
}

TEST_F(GraphCoreTest, NodeCreation)
{
    // Attempt to create a node type that doesn't exist
    EXPECT_EQ(root_.createNode("NonexistentNodeType", "Bob"), nullptr);

    // Wrong case in existing node type (should fail as createNode() defaults to requiring the strict node type)
    EXPECT_EQ(root_.createNode("add", "Bob"), nullptr);

    // Wrong case in existing node type (succeeds with non-strict type name checking)
    EXPECT_EQ(NodeRegistry::getNodeTypesFuzzy("prod")[0], "DotProduct");
}

} // namespace UnitTest
