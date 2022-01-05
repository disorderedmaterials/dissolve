// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "procedure/nodes/node.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class Species;
class PickProcedureNodeBase;

// Transmute Node
class TransmuteProcedureNode : public ProcedureNode
{
    public:
    TransmuteProcedureNode();
    ~TransmuteProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Control
     */
    private:
    // Existing selection from which to pick
    std::shared_ptr<const PickProcedureNodeBase> selection_;
    // Target species to transmute in to
    const Species *targetSpecies_;
    // Species to transmute
    std::vector<const Species *> speciesToTransmute_;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
