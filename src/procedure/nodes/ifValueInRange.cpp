// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/ifValueInRange.h"
#include "expression/variable.h"
#include "keywords/node.h"
#include "keywords/nodeBranch.h"
#include "keywords/nodeValue.h"
#include "keywords/range.h"
#include "procedure/nodes/sequence.h"
#include <algorithm>

IfValueInRangeProcedureNode::IfValueInRangeProcedureNode()
    : ProcedureNode(ProcedureNode::NodeType::IfValueInRange, {NodeContext::AnyContext}),
      thenBranch_(NodeContext::ParentProcedureContext, *this, "Then")
{
    keywords_.setOrganisation("Options", "Condition");
    keywords_.add<NodeValueKeyword>("Value", "Value expression to test", value_, this);
    keywords_.add<RangeKeyword>("ValidRange", "Valid acceptance range for value", valueRange_,
                                Vec3Labels::MinMaxBinwidthlabels);

    keywords_.addHidden<NodeBranchKeyword>("Then", "Branch to run on valid value", thenBranch_);
}

/*
 * Identity
 */

// Return whether a name for the node must be provided
bool IfValueInRangeProcedureNode::mustBeNamed() const { return false; }

/*
 * Parameters
 */

// Return the named parameter (if it exists)
std::shared_ptr<ExpressionVariable>
IfValueInRangeProcedureNode::getParameter(std::string_view name, std::shared_ptr<ExpressionVariable> excludeParameter)
{
    for (auto var : parameters_)
        if ((var != excludeParameter) && (DissolveSys::sameString(var->name(), name)))
            return var;

    return nullptr;
}

// Return vector of all parameters for this node
OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> IfValueInRangeProcedureNode::parameters() const
{
    return parameters_;
}

/*
 * Branch
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> IfValueInRangeProcedureNode::branch() { return thenBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IfValueInRangeProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    return thenBranch_.prepare(procedureContext);
}

// Execute node
bool IfValueInRangeProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // Evaluate and check
    if (valueRange_.contains(value_.asDouble()))
        if (!thenBranch_.execute(procedureContext))
            return false;

    return true;
}

// Finalise any necessary data after execution
bool IfValueInRangeProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    return thenBranch_.finalise(procedureContext);
}
