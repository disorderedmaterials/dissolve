// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/test.h"

TestNode::TestNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput<Configuration *>("Configuration", "A configuration input", configuration_);
    addInput("CreateConfiguration", "Whether to create the optional configuration on run", createConfiguration_);
    addInput("Number", "A single number", number_);
    addInput("NumberVector", "A vector of numbers", numberVector_);
    addInput("OptionalNumber", "A single number", optionalNumber_);
    addInput("Variant", "A variant", variant_);

    // Outputs
    addOutput<Configuration *>("Configuration", "A configuration output", configuration_);
    addOptionalPointerOutput<Configuration>("OptionalConfiguration", "An optional Configuration", optionalConfiguration_);
    addOutput("Number", "A single number", number_);
    addOutput("NumberVector", "A vector of numbers", numberVector_);
    addOutput("OptionalNumber", "An optional number", optionalNumber_);
    addOutput("Variant", "A variant", variant_);
}

/*
 * Definition
 */

// Return type of the node
// Return type of the node
std::string_view TestNode::type() const { return "Test"; }

// Return short summary of the node's purpose
// Return short summary of the node's purpose
std::string_view TestNode::summary() const { return "A node to allow unit testing of various graph features"; }

// Return the optional Configuration
const std::optional<Configuration> &TestNode::optionalConfiguration() const { return optionalConfiguration_; }

// Return the variant
TestNode::TestVariant TestNode::variant() { return variant_; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult TestNode::process()
{
    if (createConfiguration_)
    {
        // Create a Configuration if one doesn't already exist
        if (!optionalConfiguration_)
            optionalConfiguration_.emplace();
    }
    else
        optionalConfiguration_ = std::nullopt;

    return NodeConstants::ProcessResult::Success;
}
