// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodevalue.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"

NodeValueKeyword::NodeValueKeyword(ProcedureNode *parentNode, NodeValue value)
    : KeywordData<NodeValue>(KeywordData::NodeValueData, value)
{
    parentNode_ = parentNode;
}

NodeValueKeyword::~NodeValueKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeValueKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int NodeValueKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeValueKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    return setValue(parser.argsv(startArg));
}

// Write keyword data to specified LineParser
bool NodeValueKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.asString()))
        return false;

    return true;
}

/*
 * Set
 */

// Set the value from supplied expression text
bool NodeValueKeyword::setValue(std::string_view expressionText)
{
    // Get any variables currently in scope
    auto vars = parentNode_->parametersInScope();

    if (!data_.set(expressionText, vars))
        return false;

    set_ = true;

    return true;
}

/*
 * Conversion
 */

// Return value (as int)
int NodeValueKeyword::asInt() { return data_.asInteger(); }

// Return value (as double)
double NodeValueKeyword::asDouble() { return data_.asDouble(); }

/*
 * Object Management
 */
