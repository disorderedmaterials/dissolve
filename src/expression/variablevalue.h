// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Forward Declarations
class ExpressionVariable;

// Expression Value Node (retrieves value of an ExpressionVariable)
class ExpressionVariableValue : public ExpressionNode
{
    public:
    ExpressionVariableValue(ExpressionVariable *var = nullptr);
    ~ExpressionVariableValue();

    /*
     * Variable Target
     */
    private:
    // Variable that this node links to
    ExpressionVariable *variable_;

    public:
    // Set variable target
    void setVariable(ExpressionVariable *v);
    // Get variable target
    ExpressionVariable *variable() const;
    // Return name of variable target
    std::string_view name() const;

    /*
     * Inherited Virtuals
     */
    public:
    // Execute node
    bool execute(ExpressionValue &result);
    // Print node contents
    void nodePrint(int offset, std::string_view prefix = "");
    // Set from ExpressionValue
    bool set(ExpressionValue value);
    // Initialise node
    bool initialise();
};
