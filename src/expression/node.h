// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "expression/value.h"
#include <memory>
#include <vector>

// Forward Declarations
class Expression;

// NETA Node
class ExpressionNode
{
    public:
    ExpressionNode() = default;
    virtual ~ExpressionNode();

    /*
     * Nodes
     */
    protected:
    // Child nodes
    std::vector<std::shared_ptr<ExpressionNode>> children_;

    public:
    // Clear all nodes
    void clear();
    // Add child node
    void addChild(std::shared_ptr<ExpressionNode> node);
    // Return number of children
    int nChildren() const;
    // Duplicate this node and its contents
    virtual std::shared_ptr<ExpressionNode> duplicate() = 0;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const = 0;
};
