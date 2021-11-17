// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/nodebranch.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

NodeBranchKeyword::NodeBranchKeyword(std::shared_ptr<SequenceProcedureNode> &data, ProcedureNode *parentNode,
                                     ProcedureNode::NodeContext branchContext)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), branchContext_(branchContext)
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool NodeBranchKeyword::isDataEmpty() const { return data_ == nullptr || data_->nNodes() == 0; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeBranchKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
int NodeBranchKeyword::maxArguments() const { return 0; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeBranchKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Check that a branch hasn't already been defined
    if (data_)
        return Messenger::error("Only one {} branch may be defined in a {} node.\n", name(),
                                ProcedureNode::nodeTypes().keyword(parentNode_->type()));

    // Create and parse a new branch
    data_ = std::make_shared<SequenceProcedureNode>(branchContext_, parentNode_->scope()->procedure(), parentNode_,
                                                    fmt::format("End{}", name()));
    if (!data_->deserialise(parser, coreData))
        return false;

    return true;
}

// Write keyword data to specified LineParser
bool NodeBranchKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_ || (data_->nNodes() == 0))
        return true;

    // Write keyword name as the start of the branch
    if (!parser.writeLineF("{}{}\n", prefix, name()))
        return false;

    // Write branch information
    if (!data_->write(parser, fmt::format("{}  ", prefix)))
        return false;

    // Write end keyword based on the name
    if (!parser.writeLineF("{}End{}\n", prefix, name()))
        return false;

    return true;
}
