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
    makeSine();
    makeCosine();
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
void TestNode::makeSine()
{
    // Append sine data
    sineData_.addPoint(0, 0);
    sineData_.addPoint(0.7853981634, 0.7071067812);
    sineData_.addPoint(1.570796327, 1);
    sineData_.addPoint(2.35619449, 0.7071067812);
    sineData_.addPoint(3.141592654, 0);
    sineData_.addPoint(3.926990817, -0.7071067812);
    sineData_.addPoint(4.71238898, -1);
    sineData_.addPoint(5.497787144, -0.7071067812);
    sineData_.addPoint(6.283185307, 0);
}

//
void TestNode::makeCosine()
{
    // Append cosine data
    cosineData_.addPoint(0, 1);
    cosineData_.addPoint(0.7853981634, 0.7071067812);
    cosineData_.addPoint(1.570796327, 0);
    cosineData_.addPoint(2.35619449, -0.7071067812);
    cosineData_.addPoint(3.141592654, -1);
    cosineData_.addPoint(3.926990817, -0.7071067812);
    cosineData_.addPoint(4.71238898, 0);
    cosineData_.addPoint(5.497787144, 0.7071067812);
    cosineData_.addPoint(6.283185307, 1);
}

//
void TestNode::shiftSinusoid(Data1D &data, bool cosine)
{
    Data1D shifted;
    for (int i = 0; i < data.nValues(); i++)
    {
        auto x = i * 2 * M_PI / data.nValues();
        auto arg = x + (iteration_ * M_PI);
        shifted.addPoint(x, cosine ? cos(arg) : sin(arg));
    }
    data = shifted;
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
    shiftSinusoid(sineData_);
    shiftSinusoid(cosineData_, true);

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
