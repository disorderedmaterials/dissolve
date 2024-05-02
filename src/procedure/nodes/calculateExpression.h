// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/calculateBase.h"

// Calculate Expression
class CalculateExpressionProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateExpressionProcedureNode();
    ~CalculateExpressionProcedureNode() override = default;

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    private:
    // Expression to evaluate
    NodeValue expression_;

    public:
    // Set expression to evaluate
    bool setExpression(std::string_view expression);

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
