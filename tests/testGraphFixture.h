// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "tests/testGraph.h"

namespace UnitTest
{
class TestGraphFixture : public testing::Test
{
    public:
    TestGraphFixture() = default;
    ~TestGraphFixture() override = default;

    private:
    // Serialised graph TOML
    SerialisedValue graphTOML_;

    protected:
    // Test graph
    TestGraph testGraph_;
    // Deserialised graph
    DissolveGraph deserialisedGraph_;
    // Current graph target
    OptionalReferenceWrapper<DissolveGraph> currentGraph_;

    private:
    // Serialise the current graph to a TOML
    bool serialiseGraphToTOML()
    {
        try
        {
            testGraph_.serialise("graph", graphTOML_);
        }
        catch (std::exception &ex)
        {
            std::cout << std::format("Failed to serialise graph TOML for test {}:\n", "TODO");
            std::cout << ex.what() << std::endl;
            return false;
        }

        return true;
    }

    protected:
    // Prepare any necessary test data
    virtual void prepareTestData() = 0;
    // Perform graph construction
    virtual void constructGraph() = 0;
    // Run graph
    virtual void runGraph() = 0;
    // Perform tests on generated data
    virtual void performTests() = 0;
    // Find specified node
    template <class NodeClass> NodeClass *findNode(std::string nodeName)
    {
        EXPECT_TRUE(currentGraph_.has_value());
        auto *node = currentGraph_.value().get().findNode(nodeName);
        EXPECT_TRUE(node);
        return dynamic_cast<NodeClass *>(node);
    }

    // Go
    void go()
    {
        // Prepare test data
        ASSERT_NO_THROW(prepareTestData());

        // Set the initial graph target to the test graph
        currentGraph_ = testGraph_;

        // Construct the graph
        ASSERT_NO_THROW(constructGraph());
        // Run the graph
        ASSERT_NO_THROW(runGraph());
        // Run the tests
        ASSERT_NO_THROW(performTests());
        // Serialise graph to TOML
        ASSERT_TRUE(serialiseGraphToTOML());

        // Switch to the deserialised graph target
        currentGraph_ = deserialisedGraph_;

        // Deserialise from the stored TOML
        ASSERT_NO_THROW(deserialisedGraph_.deserialise(graphTOML_["graph"]));
        // Run the graph
        ASSERT_NO_THROW(runGraph());
        // Run the tests
        ASSERT_NO_THROW(performTests());
    }
};
}; // namespace UnitTest
