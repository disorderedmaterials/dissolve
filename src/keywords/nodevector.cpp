// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/nodevector.h"
#include "keywords/node.h"
#include "procedure/nodes/node.h"

NodeVectorKeyword::NodeVectorKeyword(ProcedureNode *parentNode, ProcedureNode::NodeType nodeType, bool onlyInScope,
                                     std::vector<const ProcedureNode *> nodes)
    : NodeKeywordBase(parentNode, nodeType, onlyInScope), KeywordData<std::vector<const ProcedureNode *>>(
                                                              KeywordBase::NodeVectorData, nodes)
{
}

NodeVectorKeyword::NodeVectorKeyword(ProcedureNode *parentNode, ProcedureNode::NodeClass nodeClass, bool onlyInScope,
                                     std::vector<const ProcedureNode *> nodes)
    : NodeKeywordBase(parentNode, nodeClass, onlyInScope), KeywordData<std::vector<const ProcedureNode *>>(
                                                               KeywordBase::NodeVectorData, nodes)
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int NodeVectorKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode())
        return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n", name());

    // Loop over arguments
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Locate the named node - don't prune by type yet (we'll check that in setNode())
        auto *node = onlyInScope() ? parentNode()->nodeInScope(parser.argsv(n)) : parentNode()->nodeExists(parser.argsv(n));
        if (!node)
            return Messenger::error("Node '{}' given to keyword {} doesn't exist.\n", parser.argsv(n), name());

        if (!validNode(node, nodeType_, nodeClass_, name()))
            return false;

        data_.push_back(node);
    }

    return true;
}

// Write keyword data to specified LineParser
bool NodeVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    std::string nodes;
    for (auto *node : data_)
        nodes += fmt::format("  '{}'", node->name());

    if (!parser.writeLineF("{}{}  {}\n", prefix, name(), nodes))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied ProcedureNode in the contained data
void NodeVectorKeyword::removeReferencesTo(ProcedureNode *node)
{
    // Check the node type
    if (node->type() != nodeType())
        return;

    data_.erase(std::remove(data_.begin(), data_.end(), node), data_.end());
}
