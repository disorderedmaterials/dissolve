// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

namespace NodeNames
{

const std::map<std::type_index, std::string_view> Subtract = {{std::type_index(typeid(double)), "SubtractDouble"},
                                                              {std::type_index(typeid(int)), "SubtractInt"}};
};

// SubractNode Node
template <typename T> class SubtractNode : public Node
{
    public:
    SubtractNode()
    {
        addInput<T>("A", "First operand to the subtraction", a_);
        addInput<T>("B", "Second operand to the subtraction, subtracted from A", b_);
        addOutput<T>("Difference", "The difference of the operands", result_);
    }
    ~SubtractNode() override = default;

    public:
    std::string_view name() const override { return "Subtract"; }
    std::string_view summary() const override { return "Performs the subtraction A - B"; }

    /*
     * Definition
     */
    private:
    // Factor A
    T a_;
    // Factor B
    T b_;
    // Product of A and B
    T result_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext)
    {
        result_ = std::minus<T>(a_, b_);
        validate();
        return Module::ExecutionResult::Success;
    }
};

using SubtractDouble = SubtractNode<double>;
using SubtractInt = SubtractNode<int>;
