// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// Test Node
class DelayNode : public Node
{
    public:
    DelayNode(Graph *parentGraph);
    ~DelayNode() override = default;

    /*
     * Definition
     */
    private:
    //
    Number delay_{2000};
    //
    Number input_{0};
    //
    Number output_{0};
    //
    bool print_{true};

    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
