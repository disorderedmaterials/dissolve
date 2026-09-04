// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"

class ConfigurationNode : public Node
{
    public:
    ConfigurationNode(Graph *parentGraph);
    ~ConfigurationNode() override = default;

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
    // Configuration object
    Configuration configuration_;
    // Save node in restart file
    std::optional<SerialisedValue> innerSaveRestart() const override;
    // Load node from restart file
    bool innerLoadRestart(SerialisedValue &data) override;

    public:
    // Return configuration object
    Configuration &configuration();

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
