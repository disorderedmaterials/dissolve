// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/nodebranch.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

NodeBranchKeyword::NodeBranchKeyword(ProcedureNodeSequence &data, ProcedureNode *parentNode,
                                     ProcedureNode::NodeContext branchContext)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), branchContext_(branchContext)
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeBranchKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
std::optional<int> NodeBranchKeyword::maxArguments() const { return 0; }

// Deserialise from supplied LineParser, starting at given argument offset
bool NodeBranchKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    return data_.deserialise(parser, coreData);
}

// Serialise data to specified LineParser
bool NodeBranchKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.nNodes() == 0)
        return true;

    // Write keyword name as the start of the branch
    if (!parser.writeLineF("{}{}\n", prefix, name()))
        return false;

    // Write branch information
    if (!data_.serialise(parser, fmt::format("{}  ", prefix)))
        return false;

    // Write end keyword based on the name
    if (!parser.writeLineF("{}End{}\n", prefix, name()))
        return false;

    return true;
}
