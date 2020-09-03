/*
    *** Expression Variable
    *** src/expression/variable.h
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
