// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/nodeValue.h"
#include "base/lineParser.h"
#include "procedure/nodes/node.h"

NodeValueKeyword::NodeValueKeyword(NodeValue &data, ProcedureNode *parentNode)
    : KeywordBase(typeid(this)), data_(data), default_(data), parentNode_(parentNode)
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
    auto vars = parentNode_->getParametersInScope();

    return data_.set(expressionText, vars);
}

// Set the value from NodeValue
bool NodeValueKeyword::setData(const NodeValue &nodeValue)
{
    // We must re-set the data from the text value of the expression to safeguard against bringing in out-of-scope parameters
    return setData(nodeValue.asString());
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

// Express as a serialisable value
SerialisedValue NodeValueKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void NodeValueKeyword::deserialise(const SerialisedValue &node, const CoreData &data)
{
    data_.deserialise(node, parentNode_->getParametersInScope());
}

// Has not changed from initial value
bool NodeValueKeyword::isDefault() const { return default_ == data_; }
