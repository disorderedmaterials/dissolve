// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "expression/functions.h"
#include "expression/node.h"

// Forward Declarations
/* none */

// Function Node
class ExpressionFunction : public ExpressionNode
{
    public:
    ExpressionFunction(ExpressionFunctions::Function func = ExpressionFunctions::NoFunction);
    ExpressionFunction(ExpressionNode *source);
    ~ExpressionFunction();

    /*
     * Function Data
     */
    protected:
    // Function that this node performs
    ExpressionFunctions::Function function_;

    public:
    // Get command function
    ExpressionFunctions::Function function() const;
    // Execute command
    bool execute(ExpressionValue &result);
    // Print node contents
    void nodePrint(int offset, std::string_view prefix = "");
    // Set from ExpressionValue
    bool set(ExpressionValue value);
    // Initialise node
    bool initialise();
};
