// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/calculatebase.h"
#include "procedure/nodevalue.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Expression
class CalculateExpression : public CalculateProcedureNodeBase
{
    public:
    CalculateExpression();
    ~CalculateExpression() override = default;
    NodeValue expression_;

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
