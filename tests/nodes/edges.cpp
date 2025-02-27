// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "module/module.h"
#include "nodes/node.h"
#include "nodes/parameter.h"
#include "nodes/parameterLink.h"

#include <gtest/gtest.h>

namespace UnitTest
{

class AddNode : public Node
{
    public:
    AddNode()
    {
        addInput<double>("A", "First Value", a);
        addInput<double>("B", "Second Value", b);
        addInput<double>("Total", "Combined Value", sum);
    }

    std::string_view name() override { return "Add Node"; }
    std::string_view summary() override { return "Add two doubles together"; };

    Module::ExecutionResult process() override
    {
        preprocess();
        sum = a + b;
        return Module::ExecutionResult::Success;
    }

    double a{0}, b{0}, sum{0};
};

TEST(NodeEdgeTest, SimpleTest)
{
    AddNode x, y, z;

    // Set-up earlier nodes
    x.a = 2;
    x.b = 3;
    y.a = 5;
    y.b = 7;
    z.a = 0;
    z.b = 0;

    // Confirm that X and Y are zero *before running*
    EXPECT_EQ(x.sum, 0);
    EXPECT_EQ(y.sum, 0);

    // Confirm that Z initially returns zero
    z.process();
    EXPECT_EQ(z.sum, 0);

    // Link the inputs of Z to X and Y
    EXPECT_TRUE(z.link("A", *x.findInput("Total")));
    EXPECT_TRUE(z.link("B", *y.findInput("Total")));

    // System should prevent double linking a sink
    EXPECT_FALSE(z.link("A", *y.findInput("Total")));

    // Confirm that we now add all the nodes
    // Note that we did *not* need to explicitly run X or Y
    z.process();
    EXPECT_EQ(z.sum, 17);
}

} // namespace UnitTest
