// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/node.h"
#include "procedure/nodes/node.h"

NodeKeyword::NodeKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope,
                         const ProcedureNode *node)
    : NodeKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<const ProcedureNode *>(KeywordBase::NodeData, node)
{
}

NodeKeyword::NodeKeyword(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope,
                         const ProcedureNode *node)
    : NodeKeywordBase(parentNode, nodeClass, onlyInScope), KeywordData<const ProcedureNode *>(KeywordBase::NodeData, node)
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int NodeKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode())
        return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n",
                                KeywordBase::name());

    // Locate the named node
    auto *node =
        onlyInScope() ? parentNode()->nodeInScope(parser.argsv(startArg)) : parentNode()->nodeExists(parser.argsv(startArg));
    if (!node)
        return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg), KeywordBase::name());

    // Check the type
    if (!validNode(node, nodeType_, nodeClass_, name()))
        return false;

    setData(node);

    return true;
}

// Write keyword data to specified LineParser
bool NodeKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // No need to write the keyword if the node pointer is null
    if (data_ == nullptr)
        return true;

    if (!parser.writeLineF("{}{}  '{}'\n", prefix, KeywordBase::name(), data_->name()))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied ProcedureNode in the contained data
void NodeKeyword::removeReferencesTo(ProcedureNode *node)
{
    if (data_ == node)
        data_ = nullptr;
}
