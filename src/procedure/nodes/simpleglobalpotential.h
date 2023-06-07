// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/simple.h"
#include "procedure/nodes/node.h"

// Simple Global Potential Procedure Node
class SimpleGlobalPotentialProcedureNode : public ProcedureNode
{
    public:
    SimpleGlobalPotentialProcedureNode();
    ~SimpleGlobalPotentialProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether specified context is relevant for this node type
    bool isContextRelevant(ProcedureNode::NodeContext context) override;

    /*
     * Potential Function
     */
    private:
    // Potential form and origin
    InteractionPotential<SimplePotentialFunctions> potential_;
    Vec3<NodeValue> origin_;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
