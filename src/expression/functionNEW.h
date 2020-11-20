// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/nodeNEW.h"

// Expression Function Node
class ExpressionFunctionNode : public ExpressionNodeNEW
{
    public:
    // Internal Functions enum
    enum InternalFunction
    {
        AbsFunction,
        ACosFunction,
        ASinFunction,
        ATanFunction,
        CosFunction,
        ExpFunction,
        LnFunction,
        LogFunction,
        SinFunction,
        SqrtFunction,
        TanFunction
    };
    // Return enum options for NodeTypes
    static EnumOptions<ExpressionFunctionNode::InternalFunction> internalFunctions();
    ExpressionFunctionNode(InternalFunction func);
    ~ExpressionFunctionNode() = default;

    /*
     * Data
     */
    private:
    // Function that the node performs
    InternalFunction function_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const;
};
