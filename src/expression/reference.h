// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Forward Declarations
class ExpressionVariable;

// Expression Variable Reference Node
class ExpressionReferenceNode : public ExpressionNode
{
    public:
    ExpressionReferenceNode(std::shared_ptr<ExpressionVariable> variable);
    ~ExpressionReferenceNode() override = default;

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
    // Target variable
    std::shared_ptr<ExpressionVariable> variable_;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    std::optional<ExpressionValue> evaluate() const override;
    // Return string representation of node
    std::string asString() const override;
};
