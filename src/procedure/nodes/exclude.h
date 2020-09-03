/*
    *** Procedure Node - Exclude
    *** src/procedure/nodes/exclude.h
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

#pragma once

#include "procedure/nodes/node.h"
#include "templates/reflist.h"

// Forward Declarations
class LineParser;
class NodeScopeStack;
class SelectProcedureNode;

// Procedure Node - Exclude
class ExcludeProcedureNode : public ProcedureNode
{
    public:
    ExcludeProcedureNode();
    ~ExcludeProcedureNode();

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context);

    /*
     * Data
     */
    private:
    // Pair of nodes (sites) to compare when disallowing same Sites
    Array<SelectProcedureNode *> sameSites_;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    ProcedureNode::NodeExecutionResult execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                               GenericList &targetList);
};
