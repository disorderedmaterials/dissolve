/*
    *** Mathematical Expression
    *** src/expression/expression.cpp
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

#include "expression/expression.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "expression/function.h"
#include "expression/generator.h"
#include "expression/value.h"
#include "expression/variable.h"
#include "expression/variablevalue.h"
#include <algorithm>
#include <stdarg.h>
#include <string.h>

Expression::Expression(std::string_view expressionText) { set(expressionText); }

Expression::~Expression() { clear(); }

Expression::Expression(const Expression &source) { (*this) = source; }

void Expression::operator=(const Expression &source)
{
    // Reset our structure, and regenerate from the expression string
    clear();

    expressionString_ = source.expressionString_;

    ExpressionGenerator::generate(*this, source.externalVariables_);
}

/*
 * Data
 */

// Clear contents of expression
void Expression::clear()
{
    nodes_.clear();
    statements_.clear();

    // Clear variables and constants, except those that are in the persistent nodes list
    std::remove_if(variables_.begin(), variables_.end(), [&](const auto *x) { return !persistentNodes_.contains(x); });

    for (auto varRef : variables_)
    {
        if (!persistentNodes_.contains(varRef))
            constants_.remove(varRef);
    }

    externalVariables_.clear();
}

// Return whether current expression is valid
bool Expression::isValid() const { return (statements_.nItems() != 0); }

// Set Expression from supplied string
bool Expression::set(std::string_view expressionString)
{
    clear();

    expressionString_ = expressionString;

    return (expressionString_.empty() ? false : ExpressionGenerator::generate(*this));
}

// Set Expression from supplied string and external variables
bool Expression::set(std::string_view expressionString, RefList<ExpressionVariable> externalVariables)
{
    clear();

    expressionString_ = expressionString;

    return (expressionString_.empty() ? false : ExpressionGenerator::generate(*this, externalVariables));
}

// Return original generating string
std::string_view Expression::expressionString() const { return expressionString_; }

/*
 * Execution
 */

// Print expression
void Expression::print()
{
    Messenger::print("Leaf Structure ({} statements):\n", statements_.nItems());
    auto n = 1;
    for (RefListItem<ExpressionNode> *ri = statements_.first(); ri != nullptr; ri = ri->next())
    {
        Messenger::print("-------------------------------------------------------------\n");
        Messenger::print("Statement {}:\n", n);
        ri->item()->nodePrint(1);
        n++;
    }
    Messenger::print("-------------------------------------------------------------\n");
}

/*
 * Statement / Command Addition
 */

// Add a node representing a whole statement to the execution list
bool Expression::addStatement(ExpressionNode *leaf)
{
    if (leaf == nullptr)
    {
        Messenger::print("Internal Error: NULL Node passed to Expression::addStatement().\n");
        return false;
    }
    leaf->setParent(this);
    statements_.append(leaf);

    return true;
}

// Add an operator to the Expression
ExpressionNode *Expression::addOperator(ExpressionFunctions::Function func, ExpressionNode *arg1, ExpressionNode *arg2)
{
    // Create new command node
    auto *leaf = new ExpressionFunction(func);
    nodes_.own(leaf);

    // Add arguments and set parent
    leaf->addArguments(1, arg1);
    leaf->setParent(this);
    if (arg2 != nullptr)
        leaf->addArguments(1, arg2);

    return leaf;
}

// Add function-based leaf node to topmost branch on stack
ExpressionNode *Expression::addFunctionNodeWithArglist(ExpressionFunctions::Function func, ExpressionNode *arglist)
{
    // Create new command node
    auto *leaf = new ExpressionFunction(func);
    nodes_.own(leaf);

    // Add argument list to node and set parent
    leaf->addJoinedArguments(arglist);
    leaf->setParent(this);

    // Check that the correct number of arguments were given to the function
    if (leaf->nArgs() != ExpressionFunctions::data[func].nArguments)
    {
        Messenger::error("Function {} requires exactly {} {}.\n", ExpressionFunctions::data[func].keyword,
                         ExpressionFunctions::data[func].nArguments,
                         ExpressionFunctions::data[func].nArguments == 1 ? "argument" : "arguments");
        return nullptr;
    }

    return leaf;
}

// Add a function node to the list (overloaded to accept simple arguments instead of a list)
ExpressionNode *Expression::addFunctionNode(ExpressionFunctions::Function func, ExpressionNode *a1, ExpressionNode *a2,
                                            ExpressionNode *a3, ExpressionNode *a4)
{
    // Create new command node
    auto *leaf = new ExpressionFunction(func);
    nodes_.own(leaf);

    if (a1 != nullptr)
        leaf->addArgument(a1);
    if (a2 != nullptr)
        leaf->addArgument(a2);
    if (a3 != nullptr)
        leaf->addArgument(a3);
    if (a4 != nullptr)
        leaf->addArgument(a4);
    leaf->setParent(this);

    // Check that the correct number of arguments were given to the function
    if (leaf->nArgs() != ExpressionFunctions::data[func].nArguments)
    {
        Messenger::error("Function {} requires exactly {} {}.\n", ExpressionFunctions::data[func].keyword,
                         ExpressionFunctions::data[func].nArguments,
                         ExpressionFunctions::data[func].nArguments == 1 ? "argument" : "arguments");
        return nullptr;
    }

    return leaf;
}

// Add value node targetting specified variable
ExpressionNode *Expression::addValueNode(ExpressionVariable *var)
{
    auto *vnode = new ExpressionVariableValue(var);
    nodes_.own(vnode);
    vnode->setParent(this);

    return vnode;
}

// Link two arguments together with their member pointers
ExpressionNode *Expression::joinArguments(ExpressionNode *arg1, ExpressionNode *arg2)
{
    arg1->prevArgument = arg2;
    arg2->nextArgument = arg1;

    return arg1;
}

// Join two commands together
ExpressionNode *Expression::joinCommands(ExpressionNode *node1, ExpressionNode *node2)
{
    auto *leaf = new ExpressionFunction(ExpressionFunctions::Joiner);
    nodes_.own(leaf);
    leaf->setParent(this);
    if (node1 != nullptr)
        leaf->addArgument(node1);
    if (node2 != nullptr)
        leaf->addArgument(node2);

    return leaf;
}

/*
 * Variables / Constants
 */

// Create numeric constant
ExpressionVariable *Expression::createConstant(ExpressionValue value, bool persistent)
{
    ExpressionVariable *var = new ExpressionVariable();
    var->set(value);
    var->setReadOnly();

    // If persistent, add to the persistent nodes list
    if (persistent)
        persistentNodes_.own(var);
    else
        nodes_.own(var);

    // Store reference
    constants_.append(var);

    return var;
}

// Create integer variable, with optional ExpressionNode as initial value source
ExpressionVariable *Expression::createIntegerVariable(std::string_view name, bool persistent, ExpressionNode *initialValue)
{
    Messenger::printVerbose("A new integer variable '{}' is being created.\n", name);

    ExpressionVariable *var = new ExpressionVariable(ExpressionValue(0));
    var->setName(name);
    if (!var->setInitialValue(initialValue))
    {
        delete var;
        Messenger::print("Failed to set initial value for integer variable.\n");
        return nullptr;
    }

    // If persistent, add to the persistent nodes list
    if (persistent)
        persistentNodes_.own(var);
    else
        nodes_.own(var);

    // Store reference
    variables_.append(var);

    Messenger::printVerbose("Created integer variable '{}'.\n", name);

    return var;
}

// Create double variable, with optional ExpressionNode as initial value source
ExpressionVariable *Expression::createDoubleVariable(std::string_view name, bool persistent, ExpressionNode *initialValue)
{
    Messenger::printVerbose("A new double variable '{}' is being created.\n", name);

    ExpressionVariable *var = new ExpressionVariable(ExpressionValue(0.0));
    var->setName(name);
    if (!var->setInitialValue(initialValue))
    {
        delete var;
        Messenger::print("Failed to set initial value for double variable.\n");
        return nullptr;
    }

    // If persistent, add to the persistent nodes list
    if (persistent)
        persistentNodes_.own(var);
    else
        nodes_.own(var);

    // Store reference
    variables_.append(var);

    Messenger::printVerbose("Created double variable '{}'.\n", name);

    return var;
}

// Create variable with supplied initial value
ExpressionVariable *Expression::createVariableWithValue(std::string_view name, ExpressionValue initialValue, bool persistent)
{
    ExpressionVariable *var = new ExpressionVariable(initialValue);
    var->setName(name);

    // If persistent, add to the persistent nodes list
    if (persistent)
        persistentNodes_.own(var);
    else
        nodes_.own(var);

    // Store reference
    variables_.append(var);

    Messenger::printVerbose("Created {} variable '{}'.\n", initialValue.isInteger() ? "integer" : "double", name);

    return var;
}

// Set list of external variables
void Expression::setExternalVariables(RefList<ExpressionVariable> externalVariables) { externalVariables_ = externalVariables; }

// Search for variable in current scope
ExpressionVariable *Expression::variable(std::string_view name)
{
    // Search external variables
    for (auto variable : externalVariables_)
        if (DissolveSys::sameString(variable->name(), name))
        {
            Messenger::printVerbose("...external variable '{}' found.\n", name);
            return variable;
        }

    // Search internal variables
    for (auto variable : variables_)
        if (DissolveSys::sameString(variable->name(), name))
            if (DissolveSys::sameString(variable->name(), name))
            {
                Messenger::printVerbose("...internal variable '{}' found.\n", name);
                return variable;
            }

    Messenger::printVerbose("...variable '{}' not found.\n", name);
    return nullptr;
}

// Return variables
RefList<ExpressionVariable> &Expression::variables() { return variables_; }

// Return constants
RefList<ExpressionVariable> &Expression::constants() { return constants_; }

/*
 * Execution
 */

// Execute expression
bool Expression::execute(ExpressionValue &result)
{
    auto success = true;
    for (RefListItem<ExpressionNode> *ri = statements_.first(); ri != nullptr; ri = ri->next())
    {
        // 		ri->item()->nodePrint(1);
        success = ri->item()->execute(result);
        if (!success)
            break;
    }

    if (!success)
        Messenger::error("Expression execution failed for '{}'.\n", expressionString_);

    return success;
}

// Execute and return as integer
int Expression::asInteger()
{
    ExpressionValue result;
    if (!execute(result))
        return 0;
    else
        return result.asInteger();
}

// Execute and return as double
double Expression::asDouble()
{
    ExpressionValue result;
    if (!execute(result))
        return 0.0;
    else
        return result.asDouble();
}
