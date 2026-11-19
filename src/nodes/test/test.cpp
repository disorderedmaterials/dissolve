// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "nodes/test/test.h"
#include <cmath>

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
    auto dx = M_PI / 4;
    sineData_.addPoint(0, sin(0));
    sineData_.addPoint(dx, sin(dx));
    sineData_.addPoint(2 * dx, sin(2 * dx));
    sineData_.addPoint(3 * dx, sin(3 * dx));
    sineData_.addPoint(4 * dx, sin(4 * dx));
    sineData_.addPoint(5 * dx, sin(5 * dx));
    sineData_.addPoint(6 * dx, sin(6 * dx));
    sineData_.addPoint(7 * dx, sin(7 * dx));
    sineData_.addPoint(8 * dx, sin(8 * dx));
    cosineData_.addPoint(0, cos(0));
    cosineData_.addPoint(dx, cos(dx));
    cosineData_.addPoint(2 * dx, cos(2 * dx));
    cosineData_.addPoint(3 * dx, cos(3 * dx));
    cosineData_.addPoint(4 * dx, cos(4 * dx));
    cosineData_.addPoint(5 * dx, cos(5 * dx));
    cosineData_.addPoint(6 * dx, cos(6 * dx));
    cosineData_.addPoint(7 * dx, cos(7 * dx));
    cosineData_.addPoint(8 * dx, cos(8 * dx));
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
void TestNode::emplaceSine(double phi)
{
    // Append sine data
    auto dx = M_PI / 4;
    const auto nPoints = 9;
    for (int i = 0; i < 9; i++)
    {
        auto x = i * dx;
        auto arg = x + phi;
        sineData_.xAxis(i) = x;
        sineData_.value(i) = sin(arg);
    }
}

//
void TestNode::emplaceCosine(double phi)
{
    // Append cosine data
    auto dx = M_PI / 4;
    const auto nPoints = 9;
    for (int i = 0; i < 9; i++)
    {
        auto x = i * dx;
        auto arg = x + phi;
        cosineData_.xAxis(i) = x;
        cosineData_.value(i) = cos(arg);
    }
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

    // Data plotting
    iteration_++;
    emplaceSine(iteration_ * (M_PI / 4) * 0.2);
    emplaceCosine(iteration_ * (M_PI / 4) * 0.2);

    message("Current iteration: {}", iteration_);

    number_ = number_ + 1;

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
