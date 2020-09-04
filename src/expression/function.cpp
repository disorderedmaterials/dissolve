/*
    *** Expression Function Node
    *** src/expression/functionnode.cpp
    Copyright T. Youngs 2015-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
