// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"
#include <memory>

class ConfigurationNode : public Node
{
    public:
    ConfigurationNode(Graph *parentGraph);
    ~ConfigurationNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Configuration object
    Configuration configuration_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};