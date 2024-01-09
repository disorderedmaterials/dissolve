// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Function Node
class ExpressionFunctionNode : public ExpressionNode
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
        PiFunction,
        RandIntRangeFunction,
        SinFunction,
        SqrtFunction,
        TanFunction,
        ToDegreesFunction,
        ToRadiansFunction,
        TwoPiFunction,
        UnitRandomFunction
    };
    // Return enum options for NodeTypes
    static EnumOptions<ExpressionFunctionNode::InternalFunction> internalFunctions();
    ExpressionFunctionNode(InternalFunction func);
    ~ExpressionFunctionNode() override = default;

    /*
     * Nodes
     */
    protected:
    // Duplicate this node and its contents
    std::shared_ptr<ExpressionNode> duplicate() override;

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
    std::optional<ExpressionValue> evaluate() const override;
    // Return string representation of node
    std::string asString() const override;
};
