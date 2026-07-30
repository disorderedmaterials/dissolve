// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/vector3.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

class InstantiateNode : public Node
{
    public:
    InstantiateNode(Graph *parentGraph);
    ~InstantiateNode() override = default;

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
    // Source Species
    const Species *species_;
    // Target configuration to insert into
    Configuration *configuration_{nullptr};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
