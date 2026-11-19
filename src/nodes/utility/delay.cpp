// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/utility/delay.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <thread>

DelayNode::DelayNode(Graph *parentGraph) : Node(parentGraph)
{
    // Inputs
    addInput("Number", "Input number", input_);

    // Outputs
    addOutput("Number", "Output number", output_);

    // Options
    addOption("Delay", "The time for which the node will sleep, in milliseconds", delay_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view DelayNode::type() const { return "Delay"; }

// Return short summary of the node's purpose
std::string_view DelayNode::summary() const { return "A node to supply a time delay, with no associated behaviour"; }

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult DelayNode::process()
{
    using namespace std::chrono_literals;

    const auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(delay_.asDouble()));
    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> elapsed = end - start;

    return NodeConstants::ProcessResult::Success;
}
