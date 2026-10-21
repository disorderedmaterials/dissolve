// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"

class ReplicatedConfigurationNode : public Node
{
    public:
    ReplicatedConfigurationNode(Graph *parentGraph);
    ~ReplicatedConfigurationNode() override = default;

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
    // Input configuration
    Configuration *configuration_{nullptr};
    // Repeat amounts
    Vector3i repeat_;
    // Supercell configuration
    Configuration replicated_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
