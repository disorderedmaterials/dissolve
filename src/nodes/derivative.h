// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "nodes/node.h"

class DerivativeNode : public Node
{
    public:
    DerivativeNode(Graph *parentGraph);
    ~DerivativeNode() override = default;

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
    // Input vector
    Data1D inputData_;
    // Derivative
    Data1D derivative_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
