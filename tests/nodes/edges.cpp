// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "module/module.h"
#include "nodes/graph.h"
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
        addOutput<double>("Total", "Combined Value", sum);
    }

    std::string_view name() const override { return "Add"; }
    std::string_view summary() const override { return "Add two doubles together"; };

    Module::ExecutionResult process() override
    {
        EXPECT_EQ(preprocess(), Node::Readiness::Ready);
        run_count++;
        sum = a + b;
        validate();
        return Module::ExecutionResult::Success;
    }

    double a{0}, b{0}, sum{0}, run_count{0};
};

TEST(NodeEdgeTest, SimpleTest)
{
    Graph graph(nullptr);

    graph.addNode(std::make_unique<AddNode>(), "x");
    graph.addNode(std::make_unique<AddNode>(), "y");
    graph.addNode(std::make_unique<AddNode>(), "z");

    Graph::Edges declaredEdges = {{"y", "Total", "z", "B"}, {"x", "Total", "z", "B"}, {"x", "Total", "z", "A"}};

    // Link the inputs of Z to X and Y
    EXPECT_TRUE(graph.addEdge(declaredEdges[0]));

    // System should prevent double linking a sink
    EXPECT_FALSE(graph.addEdge(declaredEdges[1]));
    EXPECT_TRUE(graph.addEdge(declaredEdges[2]));

    // Create simple references to the various nodes we've created
    auto &x = *static_cast<AddNode *>(graph.nodes()["x"].get());
    auto &y = *static_cast<AddNode *>(graph.nodes()["y"].get());
    auto &z = *static_cast<AddNode *>(graph.nodes()["z"].get());

    // Confirm that X and Y are zero *before running*
    EXPECT_EQ(x.sum, 0);
    EXPECT_EQ(y.sum, 0);

    EXPECT_FALSE(x.isSatisfied());
    EXPECT_FALSE(y.isSatisfied());
    EXPECT_FALSE(z.isSatisfied());

    // Confirm that Z initially returns zero
    z.process();
    EXPECT_EQ(x.sum, 0);
    EXPECT_EQ(y.sum, 0);
    EXPECT_EQ(z.sum, 0);
    EXPECT_EQ(x.run_count, 0);
    EXPECT_EQ(y.run_count, 0);
    EXPECT_EQ(z.run_count, 1);
    EXPECT_FALSE(x.isSatisfied());
    EXPECT_FALSE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    // Set parameters within module *without* explicitly invalidating
    x.a = 1;
    x.b = 9;

    // Nothing has changed, so children should not rerun
    z.process();
    EXPECT_EQ(x.sum, 0);
    EXPECT_EQ(y.sum, 0);
    EXPECT_EQ(z.sum, 0);
    EXPECT_EQ(x.run_count, 0);
    EXPECT_EQ(y.run_count, 0);
    EXPECT_EQ(z.run_count, 2);
    EXPECT_FALSE(x.isSatisfied());
    EXPECT_FALSE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    // Mark x as needing update
    x.invalidate();

    // X still should *NOT* update, because "A" is not an invalidating
    // parameter
    z.process();
    EXPECT_EQ(x.sum, 0);
    EXPECT_EQ(y.sum, 0);
    EXPECT_EQ(z.sum, 0);
    EXPECT_EQ(x.run_count, 0);
    EXPECT_EQ(y.run_count, 0);
    EXPECT_EQ(z.run_count, 3);
    EXPECT_FALSE(x.isSatisfied());
    EXPECT_FALSE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    // Declare that parameters invalidate
    x.findParameter("A")->setFlags(ParameterBase::Invalidates);
    x.findParameter("B")->setFlags(ParameterBase::Invalidates);
    y.findParameter("A")->setFlags(ParameterBase::Invalidates);
    y.findParameter("B")->setFlags(ParameterBase::Invalidates);
    z.findParameter("A")->setFlags(ParameterBase::Invalidates);
    z.findParameter("B")->setFlags(ParameterBase::Invalidates);

    // All should update
    z.process();
    EXPECT_EQ(x.sum, 10);
    EXPECT_EQ(y.sum, 0);
    EXPECT_EQ(z.sum, 10);
    EXPECT_EQ(x.run_count, 1);
    EXPECT_EQ(y.run_count, 1);
    EXPECT_EQ(z.run_count, 4);
    EXPECT_TRUE(x.isSatisfied());
    EXPECT_TRUE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    // Running the process again should NOT rerun x and y, since they
    // are satisfied
    z.process();
    EXPECT_EQ(x.sum, 10);
    EXPECT_EQ(y.sum, 0);
    EXPECT_EQ(z.sum, 10);
    EXPECT_EQ(x.run_count, 1);
    EXPECT_EQ(y.run_count, 1);
    EXPECT_EQ(z.run_count, 5);
    EXPECT_TRUE(x.isSatisfied());
    EXPECT_TRUE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    // Set the parameters with setters.  This should automatically
    // invalidate Y
    y.findParameter("A")->upcast<double>()->set(2);
    y.findParameter("B")->upcast<double>()->set(7);

    // Confirm that only Y is rerun in the recalculation of Z
    z.process();
    EXPECT_EQ(x.sum, 10);
    EXPECT_EQ(y.sum, 9);
    EXPECT_EQ(z.sum, 19);
    EXPECT_EQ(x.run_count, 1);
    EXPECT_EQ(y.run_count, 2);
    EXPECT_EQ(z.run_count, 6);
    EXPECT_TRUE(x.isSatisfied());
    EXPECT_TRUE(y.isSatisfied());
    EXPECT_TRUE(z.isSatisfied());

    Graph copy(nullptr);
    auto serialised = graph.serialise();
    std::cout << serialised << std::endl;

    // For now, expect throws because AddNode is not registered
    EXPECT_THROW(copy.deserialise(serialised), std::runtime_error);
    // EXPECT_EQ(graph.nodes(), copy.nodes());
}

} // namespace UnitTest
