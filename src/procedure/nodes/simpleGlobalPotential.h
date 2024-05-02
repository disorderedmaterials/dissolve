// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
