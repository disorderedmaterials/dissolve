// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/calculatebase.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "procedure/nodes/select.h"

CalculateProcedureNodeBase::CalculateProcedureNodeBase(ProcedureNode::NodeType nodeType, SelectProcedureNode *site0,
                                                       SelectProcedureNode *site1, SelectProcedureNode *site2,
                                                       SelectProcedureNode *site3)
    : ProcedureNode(nodeType)
{
    sites_[0] = site0;
    sites_[1] = site1;
    sites_[2] = site2;
    sites_[3] = site3;
    siteKeywords_[0] = nullptr;
    siteKeywords_[1] = nullptr;
    siteKeywords_[2] = nullptr;
    siteKeywords_[3] = nullptr;
    value_ = 0.0;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool CalculateProcedureNodeBase::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
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
    {
        sites_[n] = siteKeywords_[n] ? siteKeywords_[n]->node() : nullptr;
        if (!sites_[n])
            return Messenger::error("Observable site {} is not set.\n", n);
    }

    return true;
}
