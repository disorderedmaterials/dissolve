// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

class SetCoordinatesNode : public Node
{
    public:
    SetCoordinatesNode(Graph *parentGraph);
    ~SetCoordinatesNode() override = default;

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
    // Target configuration
    Configuration *configuration_;
    // Structure to apply
    Structure structure_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};