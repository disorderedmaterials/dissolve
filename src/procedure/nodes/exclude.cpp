// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "procedure/nodes/exclude.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/species.h"
#include "keywords/types.h"
#include "procedure/nodes/select.h"

ExcludeProcedureNode::ExcludeProcedureNode() : ProcedureNode(ProcedureNode::ExcludeNode)
{
    keywords_.add("Exclusions",
                  new NodeArrayKeyword<SelectProcedureNode>(this, ProcedureNode::SelectNode, 2, true, true, sameSites_),
                  "SameSite", "Calculated observable to collect for x axis");
}

ExcludeProcedureNode::~ExcludeProcedureNode() {}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool ExcludeProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Execute
 */

// Execute node, targetting the supplied Configuration
ProcedureNode::NodeExecutionResult ExcludeProcedureNode::execute(ProcessPool &procPool, Configuration *cfg,
                                                                 std::string_view prefix, GenericList &targetList)
{
    // Exclude based on Sites?
    if (sameSites_.at(0) && sameSites_.at(1))
    {
        if (sameSites_.at(0)->currentSite() == sameSites_.at(1)->currentSite())
            return ProcedureNode::SomethingElse;
    }

    return ProcedureNode::Success;
}
