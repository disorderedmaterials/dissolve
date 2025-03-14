// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include "math/data1D.h"

// DerivativeNode Node
class DerivativeNode : public Node
{
    public:
    DerivativeNode();
    ~DerivativeNode() override = default;

    public:
    std::string_view name() const override;
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
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
