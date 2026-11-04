// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/test/test.h"

TestNode::TestNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Configuration", "A configuration input", configuration_);
    addInput("CreateConfiguration", "Whether to create the optional configuration on run", createConfiguration_);
    addInput("Number", "A single number", number_);
    addInput("NumberVector", "A vector of numbers", numberVector_);
    addInput("OptionalNumber", "A single number", optionalNumber_);
    addInput("Variant", "A variant", variant_);
    addInput("Message", "A message", message_);
    addInput("Char", "A character", char_);
    addInput("CharPtr", "A character", charPtr_);

    // Outputs
    addOutput("Configuration", "A configuration output", configuration_);
    addOptionalPointerOutput<Configuration>("OptionalConfiguration", "An optional Configuration", optionalConfiguration_);
    addOutput("Number", "A single number", number_);
    addOutput("NumberVector", "A vector of numbers", numberVector_);
    addOutput("OptionalNumber", "An optional number", optionalNumber_);
    addOutput("Variant", "A variant", variant_);
    addOutput("Sine", "A test sine data series", sineData_);
    addOutput("Cosine", "A test cosine data series", cosineData_);

    // Fill out some test data at construction time
    makeSine(sineData_);
    makeCosine(cosineData_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view TestNode::type() const { return "Test"; }

// Return short summary of the node's purpose
std::string_view TestNode::summary() const { return "A node to allow unit testing of various graph features"; }

// Return the optional Configuration
const std::optional<Configuration> &TestNode::optionalConfiguration() const { return optionalConfiguration_; }

// Return the variant
TestNode::TestVariant TestNode::variant() { return variant_; }

//
static void TestNode::makeSine(auto &sineData)
{
    // Append sine data
    sineData.addPoint(0, 0);
    sineData.addPoint(0.7853981634, 0.7071067812);
    sineData.addPoint(1.570796327, 1);
    sineData.addPoint(2.35619449, 0.7071067812);
    sineData.addPoint(3.141592654, 0);
    sineData.addPoint(3.926990817, -0.7071067812);
    sineData.addPoint(4.71238898, -1);
    sineData.addPoint(5.497787144, -0.7071067812);
    sineData.addPoint(6.283185307, 0);
}

//
static void TestNode::makeCosine(auto &cosineData)
{
    // Append cosine data
    cosineData.addPoint(0, 1);
    cosineData.addPoint(0.7853981634, 0.7071067812);
    cosineData.addPoint(1.570796327, 0);
    cosineData.addPoint(2.35619449, -0.7071067812);
    cosineData.addPoint(3.141592654, -1);
    cosineData.addPoint(3.926990817, -0.7071067812);
    cosineData.addPoint(4.71238898, 0);
    cosineData.addPoint(5.497787144, 0.7071067812);
    cosineData.addPoint(6.283185307, 1);
}

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

    // Standard dynamic outputs
    messageParts_.clear();
    messageParts_.insert(messageParts_.end(), message_.begin(), message_.end());

    /*
     * Dynamic outputs
     */

    // Register dynamic (standard) outputs
    for (int i = 0; i < messageParts_.size(); i++)
    {
        auto val = messageParts_[i];
        auto paramName = std::string("Message-Part" + std::format("-{}", i));

        // Check if output already exists - do not add if it does
        if (outputs_.find(paramName) != outputs_.end())
            continue;

        addOutput(paramName, "Part of a message", messageParts_[i]);
    }

    // Register dynamic pointer outputs
    for (int i = 0; i < messageParts_.size(); i++)
    {
        auto val = messageParts_[i];
        auto paramName = std::string("Message-Ptr-Part" + std::format("-{}", i));

        // Check if output already exists - do not add if it does
        if (outputs_.find(paramName) != outputs_.end())
            continue;

        addPointerOutput(paramName, "Part of a message", messageParts_[i]);
    }

    return NodeConstants::ProcessResult::Success;
}
