// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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

std::string NodeValueKeyword::toString() const {
  return data_.asString();
}

// Set the value from supplied expression text
bool NodeValueKeyword::setData(std::string_view expressionText)
{
    // Get any variables currently in scope
    auto vars = parentNode_->parametersInScope();

    return data_.set(expressionText, vars);
}

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool NodeValueKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    return setData(parser.argsv(startArg));
}

// Serialise data to specified LineParser
bool NodeValueKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_.asString()))
        return false;

    return true;
}
