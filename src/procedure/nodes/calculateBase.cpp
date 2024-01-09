// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/calculateBase.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "procedure/nodes/select.h"

CalculateProcedureNodeBase::CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType,
                                                       std::shared_ptr<SelectProcedureNode> site0,
                                                       std::shared_ptr<SelectProcedureNode> site1,
                                                       std::shared_ptr<SelectProcedureNode> site2,
                                                       std::shared_ptr<SelectProcedureNode> site3)
    : ProcedureNode(nodeType, {ProcedureNode::AnalysisContext}, ProcedureNode::NodeClass::Calculate),
      sites_{site0, site1, site2, site3}, value_{0.0, 0.0, 0.0}
{
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
bool CalculateProcedureNodeBase::prepare(const ProcedureContext &procedureContext)
{
    // Check that the sites have been properly defined
    for (auto n = 0; n < nSitesRequired(); ++n)
        if (!sites_[n])
            return Messenger::error("Observable site {} is not set.\n", n);

    return true;
}
