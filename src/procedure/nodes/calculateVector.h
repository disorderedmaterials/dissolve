// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/calculateBase.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Vector Node
class CalculateVectorProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateVectorProcedureNode(std::shared_ptr<SelectProcedureNode> site0 = nullptr,
                                 std::shared_ptr<SelectProcedureNode> site1 = nullptr, bool rotateIntoFrame = false);
    ~CalculateVectorProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Whether to rotate into the frame of the first site
    bool rotateIntoFrame_;

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
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
