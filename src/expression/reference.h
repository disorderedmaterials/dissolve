// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Forward Declarations
class ExpressionVariable;

// Expression Variable Reference Node
class ExpressionReferenceNode : public ExpressionNode
{
    public:
    ExpressionReferenceNode(ExpressionVariable *variable);
    ~ExpressionReferenceNode() = default;

    /*
     * Data
     */
    private:
    // Target variable
    ExpressionVariable *variable_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const;
};
