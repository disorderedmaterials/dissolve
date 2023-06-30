// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/directional.h"
#include "procedure/nodes/node.h"

// Directional Global Potential Procedure Node
class DirectionalGlobalPotentialProcedureNode : public ProcedureNode
{
    public:
    DirectionalGlobalPotentialProcedureNode();
    ~DirectionalGlobalPotentialProcedureNode() override = default;

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
    // Potential form, origin and directional vector
    InteractionPotential<DirectionalPotentialFunctions> potential_;
    Vec3<NodeValue> origin_;
    Vec3<NodeValue> vector_;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
