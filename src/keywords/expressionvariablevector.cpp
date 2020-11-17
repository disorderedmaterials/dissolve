// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "keywords/expressionvariablevector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "expression/variable.h"
#include "procedure/nodes/node.h"
#include <memory>

ExpressionVariableVectorKeyword::ExpressionVariableVectorKeyword(ProcedureNode *parentNode,
                                                                 std::vector<std::shared_ptr<ExpressionVariable>> &variables,
                                                                 ExpressionValue::ValueType variableType)
    : KeywordData<std::vector<std::shared_ptr<ExpressionVariable>> &>(KeywordBase::ExpressionVariableVectorData, variables)
{
    parentNode_ = parentNode;
    variableType_ = variableType;
}

ExpressionVariableVectorKeyword::~ExpressionVariableVectorKeyword() {}

/*
 * Parent Node
 */

// Return parent ProcedureNode
const ProcedureNode *ExpressionVariableVectorKeyword::parentNode() const { return parentNode_; }

/*
 * Variable Type
 */

// Return assumed type for variables in the list
ExpressionValue::ValueType ExpressionVariableVectorKeyword::variableType() const { return variableType_; }

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
bool ExpressionVariableVectorKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
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

    // Set the initial value
    parameter->setValue(variableType_ == ExpressionValue::IntegerType ? parser.argi(startArg + 1) : parser.argd(startArg + 1));

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ExpressionVariableVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of defined ExpressionNode's (ExpressionVariables)
    for (const auto node : data_)
    {
        if (variableType_ == ExpressionValue::IntegerType)
        {
            if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, node->name(), node->value().asInteger()))
                return false;
        }
        else if (variableType_ == ExpressionValue::DoubleType)
        {
            if (!parser.writeLineF("{}{}  {}  {:12.6e}\n", prefix, keywordName, node->name(), node->value().asDouble()))
                return false;
        }
    }

    return true;
}

/*
 * Object Management
 */
