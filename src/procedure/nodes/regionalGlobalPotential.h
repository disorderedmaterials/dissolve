// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/directional.h"
#include "procedure/nodes/node.h"

// Directional Global Potential Procedure Node
class RegionalGlobalPotentialProcedureNode : public ProcedureNode
{
    public:
    RegionalGlobalPotentialProcedureNode();
    ~RegionalGlobalPotentialProcedureNode() override = default;

    /*
     * Potential Function
     */
    private:

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
