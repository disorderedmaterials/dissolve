// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class Species;
class PickProcedureNodeBase;

// Remove Node
class RemoveProcedureNode : public ProcedureNode
{
    public:
    RemoveProcedureNode();
    ~RemoveProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Data
     */
    private:
    // Selection of molecules to remove
    const PickProcedureNodeBase *selection_;
    // Species to remove
    std::vector<const Species *> speciesToRemove_;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
