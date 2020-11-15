// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Expression Root Node
class ExpressionRootNode : public ExpressionNode
{
    public:
    ExpressionRootNode();
    ~ExpressionRootNode() = default;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const;
};
