/*
    *** Keyword - NodeBranch
    *** src/keywords/nodebranch.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/nodebranch.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

NodeBranchKeyword::NodeBranchKeyword(ProcedureNode *parentNode, SequenceProcedureNode **branchPointer,
                                     ProcedureNode::NodeContext branchContext)
    : KeywordData<SequenceProcedureNode **>(KeywordData::NodeBranchData, branchPointer)
{
    parentNode_ = parentNode;
    branchContext_ = branchContext;
}

NodeBranchKeyword::~NodeBranchKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool NodeBranchKeyword::isDataEmpty() const { return ((*data_) ? (*data_)->nNodes() > 0 : false); }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeBranchKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
int NodeBranchKeyword::maxArguments() const { return 0; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool NodeBranchKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Check that a branch hasn't already been defined
    if (*data_)
        return Messenger::error("Only one {} branch may be defined in a {} node.\n", name(),
                                ProcedureNode::nodeTypes().keyword(parentNode_->type()));

    // Create and parse a new branch
    (*data_) =
        new SequenceProcedureNode(branchContext_, parentNode_->scope()->procedure(), parentNode_, fmt::format("End{}", name()));
    if (!(*data_)->read(parser, coreData))
        return false;

    return true;
}

// Write keyword data to specified LineParser
bool NodeBranchKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    if (!(*data_) || ((*data_)->nNodes() == 0))
        return true;

    // Write keyword name as the start of the branch
    if (!parser.writeLineF("{}{}\n", prefix, name()))
        return false;

    // Write branch information
    if (!(*data_)->write(parser, fmt::format("{}  ", prefix)))
        return false;

    // Write end keyword based on the name
    if (!parser.writeLineF("{}End{}\n", prefix, name()))
        return false;

    return true;
}
