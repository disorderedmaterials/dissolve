// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

// AddNode Node
template <typename T> class AddNode : public Node
{
    public:
    AddNode()
    {
        addInput<T>("A", "First operand to the addition", a_);
        addInput<T>("B", "Second operand to the addition", b_);
    }
    ~AddNode() override = default;

    public:
    std::string_view name() override { return "Add"; }
    std::string_view summary() override { return "Performs addition of operands A and B"; }

    /*
     * Definition
     */
    private:
    // Operand A
    T a_;
    // Operand B
    T b_;
    // Sum of A and B
    T sum_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext)
    {
        sum_ = std::plus<T>(a_, b_);

        return ExecutionResult::Success;
    }
};
