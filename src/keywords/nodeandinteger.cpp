// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodeandinteger.h"
#include "procedure/nodes/node.h"

NodeAndIntegerKeyword::NodeAndIntegerKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope,
                                             const ProcedureNode *node, int index)
    : NodeKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<std::pair<const ProcedureNode *, int>>({node, index})
{
}

NodeAndIntegerKeyword::NodeAndIntegerKeyword(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope,
                                             const ProcedureNode *node, int index)
    : NodeKeywordBase(parentNode, nodeClass, onlyInScope), KeywordData<std::pair<const ProcedureNode *, int>>({node, index})
{
}

/*
 * Arguments
 */

// Return maximum number of arguments accepted
int NodeAndIntegerKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool NodeAndIntegerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode())
        return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                KeywordBase::name());

    // Locate the named node in scope - don't prune by type yet (we'll check that in setNode())
    auto *node =
        onlyInScope() ? parentNode()->nodeInScope(parser.argsv(startArg)) : parentNode()->nodeExists(parser.argsv(startArg));
    if (!node)
        return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg), KeywordBase::name());

    // Check the type
    if (!validNode(node, nodeType_, nodeClass_, name()))
        return false;

    if (parser.hasArg(startArg + 1))
        setData({node, parser.argi(startArg + 1)});
    else
        setData({node, 0});

    return true;
}

// Serialise data to specified LineParser
bool NodeAndIntegerKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'  {}\n", prefix, KeywordBase::name(), data_.first ? data_.first->name() : "???",
                             data_.second);
}

/*
 * Object Management
 */

// Prune any references to the supplied ProcedureNode in the contained data
void NodeAndIntegerKeyword::removeReferencesTo(ProcedureNode *node)
{
    if (data_.first == node)
        data_.first = nullptr;
}
