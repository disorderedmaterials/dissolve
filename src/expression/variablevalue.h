/*
    *** Expression Variable Value Node
    *** src/expression/variablevalue.h
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
