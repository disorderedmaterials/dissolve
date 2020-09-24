// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/function.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <stdarg.h>
#include <string.h>

ExpressionFunction::ExpressionFunction(ExpressionFunctions::Function func) : ExpressionNode(), function_(func)
{
    // Private variables
    nodeType_ = ExpressionNode::FunctionNode;
}

ExpressionFunction::ExpressionFunction(ExpressionNode *source) { copy(source); }

ExpressionFunction::~ExpressionFunction() {}

// Get function
ExpressionFunctions::Function ExpressionFunction::function() const { return function_; }

// Execute command
bool ExpressionFunction::execute(ExpressionValue &result) { return expressionFunctions.call(function_, this, result); }

// Print node contents
void ExpressionFunction::nodePrint(int offset, std::string_view prefix)
{
    // Construct tabbed offset
    std::string tab;
    for (int n = 0; n < offset - 1; n++)
        tab += '\t';
    if (offset > 1)
        tab += "   |--> ";
    tab += prefix;

    // Output node data
    Messenger::print("[CN]{}{} (Function) ({} arguments)\n", tab, ExpressionFunctions::data[function_].keyword, args_.nItems());
    // Output Argument data
    for (RefListItem<ExpressionNode> *ri = args_.first(); ri != nullptr; ri = ri->next())
        ri->item()->nodePrint(offset + 1);
}

// Set from ExpressionValue
bool ExpressionFunction::set(ExpressionValue value)
{
    Messenger::error("Internal Error: Trying to 'set' a FunctionNode.\n");
    return false;
}

// Initialise node
bool ExpressionFunction::initialise()
{
    Messenger::error("Internal Error: A FunctionNode cannot be initialised.\n");
    return false;
}
