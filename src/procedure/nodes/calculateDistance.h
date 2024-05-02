// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/calculateBase.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Distance Node
class CalculateDistanceProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateDistanceProcedureNode(std::shared_ptr<SelectProcedureNode> site0 = nullptr,
                                   std::shared_ptr<SelectProcedureNode> site1 = nullptr);
    ~CalculateDistanceProcedureNode() override = default;

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    public:
    // Return number of sites required to calculate observable
    int nSitesRequired() const override;
    // Return dimensionality of calculated observable
    int dimensionality() const override;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
