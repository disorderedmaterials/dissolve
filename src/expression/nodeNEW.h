// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "expression/value.h"
#include <memory>
#include <vector>

// Forward Declarations
class ExpressionNEW;

// NETA Node
class ExpressionNodeNEW
{
    public:
    ExpressionNodeNEW() = default;
    virtual ~ExpressionNodeNEW();

    /*
     * Nodes
     */
    protected:
    // Child nodes
    std::vector<std::shared_ptr<ExpressionNodeNEW>> children_;

    public:
    // Clear all nodes
    void clear();
    // Add child node
    void addChild(std::shared_ptr<ExpressionNodeNEW> node);
    // Return number of children
    int nChildren() const;

    /*
     * Evaluation
     */
    public:
    // Evaluate node
    virtual std::optional<ExpressionValue> evaluate() const = 0;
};
