// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodevalue.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"

NodeValueKeyword::NodeValueKeyword(NodeValue &data, ProcedureNode *parentNode)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode)
{
}

/*
 * Data
 */

// Return reference to data
NodeValue &NodeValueKeyword::data() { return data_; }
const NodeValue &NodeValueKeyword::data() const { return data_; }

// Set the value from supplied expression text
bool NodeValueKeyword::setData(std::string_view expressionText)
{
    // Get any variables currently in scope
    auto vars = parentNode_->parametersInScope();

    if (!data_.set(expressionText, vars))
        return false;

    set_ = true;

    return true;
}

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
    return setData(parser.argsv(startArg));
}

// Write keyword data to specified LineParser
bool NodeValueKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.asString()))
        return false;

    return true;
}
