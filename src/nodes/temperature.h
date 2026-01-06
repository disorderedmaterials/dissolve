// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"
#include "nodes/number.h"

// Temperature Node
class TemperatureNode : public Node
{
    public:
    explicit TemperatureNode(Graph *parentGraph);
    ~TemperatureNode() override = default;

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // New configuration temperature value
    Number temperature_;
    // Target configuration for which the temperature will be changed
    Configuration *targetConfiguration_;

    /*
     * Processing
     */
    public:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};