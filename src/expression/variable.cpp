/*
    *** Expression Variable
    *** src/expression/variable.cpp
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

#include "expression/variable.h"
#include "base/messenger.h"
#include <string.h>

ExpressionVariable::ExpressionVariable(ExpressionValue value, bool readOnly) : ExpressionNode()
{
    // Private variables
    static int count = 0;
    name_ = fmt::format("_ExpressionVariable{:02d}", count++);
    initialValue_ = nullptr;
    nodeType_ = ExpressionNode::VariableNode;
    value_ = value;
    readOnly_ = readOnly;
}

// Destructor (virtual)
ExpressionVariable::~ExpressionVariable() {}

// Set name of variable
void ExpressionVariable::setName(std::string_view s) { name_ = s; }

// Get name of variable
std::string_view ExpressionVariable::name() const { return name_; }

// Initialise variable
bool ExpressionVariable::initialise()
{
    if (initialValue_ == nullptr)
        value_ = 0.0;
    else
    {
        ExpressionValue result;
        if (initialValue_->execute(result))
        {
            if (set(result))
                return true;
            else
            {
                Messenger::error("Failed to initialise variable '{}'.\n", name_);
                return false;
            }
        }
        return false;
    }
    return true;
}

// Set initial value expression
bool ExpressionVariable::setInitialValue(ExpressionNode *node)
{
    initialValue_ = node;
    if (initialValue_ == nullptr)
        return true;

    return true;
}

// Return Node corresponding to initial value
ExpressionNode *ExpressionVariable::initialValue() const { return initialValue_; }

/*
 * Set / Get
 */

// Set value of variable (real)
bool ExpressionVariable::set(ExpressionValue value)
{
    if (readOnly_)
        return Messenger::error("A constant value (in this case a double) cannot be assigned to.\n");

    value_ = value;

    return true;
}

// Return value
const ExpressionValue &ExpressionVariable::value() const { return value_; }

// Return pointer to value
ExpressionValue *ExpressionVariable::valuePointer() { return &value_; }

// Return value of node
bool ExpressionVariable::execute(ExpressionValue &value)
{
    value = value_;

    return true;
}

// Print node contents
void ExpressionVariable::nodePrint(int offset, std::string_view prefix)
{
    // Construct tabbed offset
    std::string tab;
    for (int n = 0; n < offset - 1; n++)
        tab += '\t';
    if (offset > 1)
        tab += "   |--> ";
    tab += prefix;

    // Output node data
    if (readOnly_)
    {
        if (value_.isInteger())
            Messenger::print("[C]{}{} (constant integer value)\n", tab, value_.asInteger());
        else
            Messenger::print("[C]{}{} (constant double value)\n", tab, value_.asDouble());
    }
    else
    {
        if (value_.isInteger())
            Messenger::print("[V]{}{} (integer variable '{}')\n", tab, value_.asInteger(), name());
        else
            Messenger::print("[V]{}{} (double variable '{}')\n", tab, value_.asDouble(), name());
    }
}
