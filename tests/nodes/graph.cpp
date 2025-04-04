// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/graph.h"
#include "nodes/add.h"
#include "nodes/dissolve.h"
#include "nodes/edge.h"
#include "nodes/number.h"
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

        // Create nodes
        x_ = dynamic_cast<AddNode *>(root_.addNode("Add", "x"));
        y_ = dynamic_cast<AddNode *>(root_.addNode("Add", "y"));
        z_ = dynamic_cast<AddNode *>(root_.addNode("Add", "z"));

        ASSERT_TRUE(x_);
        ASSERT_TRUE(y_);
        ASSERT_TRUE(z_);

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

// Helper function for comparing TOML values with context, but without
// insisting on a specific ordering of fields.
void compare_toml(std::string location, SerialisedValue toml, SerialisedValue toml2)
{
    if (toml.is_table())
    {
        ASSERT_TRUE(toml2.is_table()) << location;
        for (auto &[k, v] : toml.as_table())
        {
            ASSERT_TRUE(toml2.contains(k)) << location << "." << k << std::endl << "Expected:" << std::endl << toml[k];
            compare_toml(std::format("{}.{}", location, k), v, toml2.at(k));
        }
    }
    else if (toml.is_array())
    {
        auto arr = toml.as_array();
        auto arr2 = toml2.as_array();
        ASSERT_EQ(arr.size(), arr2.size()) << location << std::endl << "Expected" << std::endl << toml;
        for (int i = 0; i < arr.size(); ++i)
            compare_toml(std::format("{}[{}]", location, i), arr[i], arr2[i]);
    }
    else
    {
        EXPECT_EQ(toml, toml2) << location;
    }
}

TEST_F(GraphCoreTest, Serialisation)
{
    createGraph();

    CoreData cd;
    Dissolve d(cd);
    DissolveGraph copy(d);
    auto serialised = root_.serialise();

    SerialisedValue contents = toml::parse("dissolve/input/simple_addition_graph.toml");
    compare_toml("", serialised, contents);

    std::cout << serialised << std::endl;
    copy.deserialise(serialised);
    auto repeat = copy.serialise();

    compare_toml("", repeat, contents);
};

} // namespace UnitTest
