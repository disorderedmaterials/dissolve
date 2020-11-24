// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/nodeNEW.h"
#include "expression/value.h"

// Expression Number Node
class ExpressionNumberNode : public ExpressionNodeNEW
{
    public:
    ExpressionNumberNode(int i);
    ExpressionNumberNode(double d);
    ~ExpressionNumberNode() = default;

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
