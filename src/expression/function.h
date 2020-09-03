/*
    *** Expression Function Node
    *** src/expression/function.h
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
