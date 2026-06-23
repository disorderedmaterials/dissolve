// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "nodes/node.h"

// SupercellConfiguration Node
class SupercellConfigurationNode : public Node
{
    public:
    SupercellConfigurationNode(Graph *parentGraph);
    ~SupercellConfigurationNode() override = default;

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
    Configuration *targetConfiguration_;
    // Supercell repeat
    Vector3i supercellRepeat_;
    // Supercell configuration
    Configuration supercellConfiguration_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
