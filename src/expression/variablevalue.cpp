// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "expression/variablevalue.h"
#include "base/messenger.h"
#include "expression/variable.h"

ExpressionVariableValue::ExpressionVariableValue(ExpressionVariable *var) : ExpressionNode(), variable_(var)
{
    // Private variables
    readOnly_ = false;
    nodeType_ = ExpressionNode::VariableValueNode;
}

ExpressionVariableValue::~ExpressionVariableValue() {}

// Set function
void ExpressionVariableValue::setVariable(ExpressionVariable *variable) { variable_ = variable; }

// Get function
ExpressionVariable *ExpressionVariableValue::variable() const { return variable_; }

// Return name of variable target
std::string_view ExpressionVariableValue::name() const
{
    if (variable_ == nullptr)
    {
        Messenger::error("ExpressionVariableValue contains a NULL Variable pointer.\n");
        return "NULL";
    }
    return variable_->name();
}

/*
 * Set / execute
 */

// Execute command
bool ExpressionVariableValue::execute(ExpressionValue &result)
{
    if (variable_ == nullptr)
    {
        Messenger::error("ExpressionVariableValue contains a NULL Variable pointer and can't be executed.\n");
        return false;
    }

    // Call the local variable's execute() function to get the base value
    auto success = variable_->execute(result);
    if (!success)
        Messenger::error("Variable retrieval ('{}') failed.\n", variable_->name());

    return success;
}

// Print node contents
void ExpressionVariableValue::nodePrint(int offset, std::string_view prefix)
{
    if (variable_ == nullptr)
    {
        Messenger::error("ExpressionVariableValue contains a NULL Variable pointer and can't be printed.\n");
        return;
    }
    // Call the local variables nodePrint() function
    variable_->nodePrint(offset, prefix);
}

// Set from ExpressionValue
bool ExpressionVariableValue::set(ExpressionValue value)
{
    if (variable_ == nullptr)
    {
        Messenger::error("ExpressionVariableValue contains a NULL Variable pointer and can't be set.\n");
        return false;
    }
    auto result = true;

    // Call the local variable's set() function
    result = variable_->set(value);
    if (!result)
        Messenger::error("Variable set failed.\n");

    return result;
}

// Initialise node
bool ExpressionVariableValue::initialise()
{
    if (variable_ == nullptr)
    {
        Messenger::error("ExpressionVariableValue contains a NULL Variable pointer and can't be initialised.\n");
        return false;
    }
    return variable_->initialise();
}
