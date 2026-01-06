// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"

// Test Node
class TestNode : public Node
{
    public:
    TestNode(Graph *parentGraph);
    ~TestNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Configuration pointer input
    Configuration *configurationInput_{nullptr};
    // Optional configuration output
    std::optional<Configuration> optionalConfiguration_;
    // Whether our processing loop creates a valid optional Configuration data
    bool createConfiguration_{false};
    // Number vector
    std::vector<Number> numberVector_;

    public:
    // Return the optional Configuration
    const std::optional<Configuration> &optionalConfiguration() const;

    /*
     * Processing & Validity
     */
    public:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
