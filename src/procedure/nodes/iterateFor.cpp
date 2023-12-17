// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/iterateFor.h"
#include "keywords/nodeBranch.h"

IterateForProcedureNode::IterateForProcedureNode() : ProcedureNode(ProcedureNode::NodeType::IterateFor, {NodeContext::AnyContext}), iterateBranch_(NodeContext::InheritContext, *this, "Iterate")
{
    keywords_.setOrganisation("Options", "Iterate");
    keywords_.add<IntegerKeyword>("nIterations", "Number of iterations to perform", IntegerKeyword(nIterations_));

    keywords_.addHidden<NodeBranchKeyword>("Then", "Branch to iterate", iterateBranch_);

}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool IterateForProcedureNode::mustBeNamed() const { return false; }

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> IterateForProcedureNode::branch() { return iterateBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IterateForProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    return iterateBranch_.prepare(procedureContext);
}

// Execute node
bool IterateForProcedureNode::execute(const ProcedureContext &procedureContext)
{
    if (iteration_ < nIterations_)
    {
        iteration_++;
        return iterateBranch_.execute(procedureContext);
    }
    return true;
}


// Finalise any necessary data after execution
bool IterateForProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    return iterateBranch_.finalise(procedureContext);
}
