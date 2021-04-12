// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/expressionvariablevector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/node.h"
#include <memory>

ExpressionVariableVectorKeyword::ExpressionVariableVectorKeyword(ProcedureNode *parentNode,
                                                                 std::vector<std::shared_ptr<ExpressionVariable>> &variables)
    : KeywordData<std::vector<std::shared_ptr<ExpressionVariable>> &>(KeywordBase::ExpressionVariableVectorData, variables)
{
    parentNode_ = parentNode;
}

ExpressionVariableVectorKeyword::~ExpressionVariableVectorKeyword() {}

/*
 * Parent Node
 */

// Return parent ProcedureNode
const ProcedureNode *ExpressionVariableVectorKeyword::parentNode() const { return parentNode_; }

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ExpressionVariableVectorKeyword::isDataEmpty() const { return data_.size() == 0; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ExpressionVariableVectorKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int ExpressionVariableVectorKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ExpressionVariableVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode_)
        return Messenger::error("Parent ProcedureNode not set, so can't read ExpressionVariableVector data.\n");

    // First argument is the name of the parameter to create - does it already exist in the node's current scope?
    auto parameter = parentNode_->parameterInScope(parser.argsv(startArg));
    if (parameter)
        return Messenger::error("A parameter with the name '{}' is already in scope, and cannot be redefined.\n",
                                parser.argsv(startArg));

    // Create a new one
    parameter = std::make_shared<ExpressionVariable>();
    data_.emplace_back(parameter);
    parameter->setName(parser.argsv(startArg));

    // Set the value
    bool isFloatingPoint = false;
    if (DissolveSys::isNumber(parser.argsv(startArg + 1), isFloatingPoint))
    {
        if (isFloatingPoint)
            parameter->setValue(parser.argd(startArg + 1));
        else
            parameter->setValue(parser.argi(startArg + 1));
    }
    else
        return Messenger::error("Value '{}' provided for variable '{}' doesn't appear to be a number.\n",
                                parser.argsv(startArg + 1), parser.argsv(startArg));

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ExpressionVariableVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over list of defined ExpressionNode's (ExpressionVariables)
    for (const auto node : data_)
    {
        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, node->name(), node->value().asString()))
            return false;
    }

    return true;
}

/*
 * Object Management
 */
