// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

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
                                               GenericList &targetList) override;
};
