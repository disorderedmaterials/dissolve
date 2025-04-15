// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <memory>
#include "classes/configuration.h"
#include "nodes/node.h"

class ConfigurationNode : public Node
{
    public:
    ConfigurationNode(Graph *parentGraph);
    ~ConfigurationNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Empty atomic configuration
    std::unique_ptr<Configuration> configuration_{std::make_unique<Configuration>()};
    // Local raw pointer to configuration
    Configuration *rawConfiguration_{configuration_.get()};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};