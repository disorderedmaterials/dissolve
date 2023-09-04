// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "kernels/potentials/regional.h"
#include "procedure/nodes/node.h"

// Regional Global Potential Procedure Node
class RegionalGlobalPotentialProcedureNode : public RegionalPotentialVoxelKernel, public ProcedureNode
{
    public:
    RegionalGlobalPotentialProcedureNode();
    ~RegionalGlobalPotentialProcedureNode() override = default;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
