// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/calculatebase.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "procedure/nodes/select.h"

CalculateProcedureNodeBase::CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType,
                                                       std::shared_ptr<SelectProcedureNode> site0,
                                                       std::shared_ptr<SelectProcedureNode> site1,
                                                       std::shared_ptr<SelectProcedureNode> site2,
                                                       std::shared_ptr<SelectProcedureNode> site3)
    : ProcedureNode(nodeType, ProcedureNode::NodeClass::Calculate), sites_{site0, site1, site2, site3}, value_{0.0, 0.0, 0.0}
{
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool CalculateProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

// Find the nodes owned by this node
std::vector<ConstNodeRef> CalculateProcedureNodeBase::children() const
{
    std::vector<ConstNodeRef> result;
    auto it = std::find_if(sites_.begin(), sites_.end(), [](const auto site) { return site == nullptr; });
    std::transform(sites_.begin(), it, std::back_inserter(result),
                   [](const auto site) { return std::static_pointer_cast<const ProcedureNode>(site); });
    return result;
}

/*
 * Observable Target
 */

// Return last calculated value of observable
double CalculateProcedureNodeBase::value(int id) const { return value_.get(id); }

// Return last calculated value of observable
Vec3<double> CalculateProcedureNodeBase::values() const { return value_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool CalculateProcedureNodeBase::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Check that the sites have been properly defined
    for (auto n = 0; n < nSitesRequired(); ++n)
        if (!sites_[n])
            return Messenger::error("Observable site {} is not set.\n", n);

    return true;
}
