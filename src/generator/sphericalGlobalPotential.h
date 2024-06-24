// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "kernels/potentials/spherical.h"

// Spherical Global Potential Procedure Node
class SphericalGlobalPotentialProcedureNode : public ProcedureNode
{
    public:
    SphericalGlobalPotentialProcedureNode();
    ~SphericalGlobalPotentialProcedureNode() override = default;

    /*
     * Potential Function
     */
    private:
    // Potential form
    InteractionPotential<Functions1D> potential_;
    // Origin coordinates
    Vec3<NodeValue> origin_;
    bool originIsFractional_{false};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
