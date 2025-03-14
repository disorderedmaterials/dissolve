// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

namespace NodeNames
{

const std::map<std::type_index, std::string_view> Add = {{std::type_index(typeid(double)), "AddDouble"},
                                                         {std::type_index(typeid(int)), "AddInt"}};
};

// AddNode Node
template <typename T> class AddNode : public Node
{
    public:
    AddNode()
    {
        addInput<T>("A", "First operand to the addition", a_);
        addInput<T>("B", "Second operand to the addition", b_);
        addOutput<T>("Total", "The sum of the operands", sum_);
    }
    ~AddNode() override = default;

    public:
    std::string_view name() const override { return NodeNames::Add.at(std::type_index(typeid(T))); }
    std::string_view summary() const override { return "Performs addition of operands A and B"; }

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
        validate();
        return Module::ExecutionResult::Success;
    }
};

using AddDouble = AddNode<double>;
using AddInt = AddNode<int>;
