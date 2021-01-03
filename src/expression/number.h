// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "expression/value.h"

// Expression Number Node
class ExpressionNumberNode : public ExpressionNode
{
    public:
    ExpressionNumberNode(int i);
    ExpressionNumberNode(double d);
    ExpressionNumberNode(const ExpressionValue &value);
    ~ExpressionNumberNode() = default;

    /*
     * Nodes
     */
    protected:
    // Duplicate this node and its contents
    std::shared_ptr<ExpressionNode> duplicate();

    /*
     * Data
     */
    private:
    // Numerical value of the node
    ExpressionValue value_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const;
};
