// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/node.h"

// Variable
class ExpressionVariable : public ExpressionNode
{
    public:
    ExpressionVariable(ExpressionValue value = ExpressionValue(), bool readOnly = false);
    ~ExpressionVariable();

    /*
     * Variable Data
     */
    protected:
    // Name of the variable
    std::string name_;
    // Value of variable
    ExpressionValue value_;
    // Initial value of new variable
    ExpressionNode *initialValue_;

    public:
    // Set name of variable
    void setName(std::string_view s);
    // Get name of variable
    std::string_view name() const;
    // Set initial value expression
    bool setInitialValue(ExpressionNode *node);
    // Return Node corresponding to initial value
    ExpressionNode *initialValue() const;

    /*
     * Set / Get
     */
    public:
    // Return value of node
    bool execute(ExpressionValue &value);
    // Set value of node
    bool set(ExpressionValue value);
    // Return value
    const ExpressionValue &value() const;
    // Return pointer to value
    ExpressionValue *valuePointer();

    /*
     * Inherited Virtuals
     */
    public:
    // Print node contents
    void nodePrint(int offset, std::string_view prefix = "");
    // Initialise node
    bool initialise();
};
