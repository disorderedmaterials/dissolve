// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "math/data1D.h"
#include "nodes/node.h"

// Derivative1D Node
class DerivativeNode : public Node
{
    public:
    DerivativeNode(Graph *parentGraph);
    ~DerivativeNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Input vector
    Data1D inputData_;
    // Derivative
    Data1D derivative_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
