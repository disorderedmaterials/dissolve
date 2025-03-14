// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

namespace NodeNames
{
const std::map<std::type_index, std::string_view> Multiply = {{std::type_index(typeid(double)), "MultiplyDouble"},
                                                              {std::type_index(typeid(int)), "MultiplyIn t"}};
};

// MultiplyNode Node
template <typename T> class MultiplyNode : public Node
{
    public:
    MultiplyNode()
    {
        addInput<T>("A", "First factor to the multiplication", a_);
        addInput<T>("B", "Second factor to the multiplication", b_);
        addOutput<T>("Product", "The product of the two factors", product_);
    }
    ~MultiplyNode() override = default;

    public:
    std::string_view name() const override { return NodeNames::Multiply.at(std::type_index(typeid(T))); }
    std::string_view summary() const override { return "Performs multiplication of factors A and B"; }

    /*
     * Definition
     */
    private:
    // Factor A
    T a_;
    // Factor B
    T b_;
    // Product of A and B
    T product_;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext)
    {
        product_ = std::multiplies<T>(a_, b_);
        validate();
        return Module::ExecutionResult::Success;
    }
};

using MultiplyDouble = MultiplyNode<double>;
using MultiplyInt = MultiplyNode<int>;
