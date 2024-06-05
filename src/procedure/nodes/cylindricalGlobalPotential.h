// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
     * Potential Function
     */
    private:
    // Potential form
    InteractionPotential<Functions1D> potential_;
    // Origin coordinates
    Vec3<NodeValue> origin_;
    bool originIsFractional_{false};
    // Directional vector
    Vec3<NodeValue> vector_{0.0, 0.0, 1.0};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
