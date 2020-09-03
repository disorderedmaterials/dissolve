/*
    *** Procedure Node - Exclude
    *** src/procedure/nodes/exclude.cpp
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
