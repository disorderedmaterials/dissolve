// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/calculateBase.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Angle Node
class CalculateAngleProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateAngleProcedureNode(std::shared_ptr<SelectProcedureNode> site0 = nullptr,
                                std::shared_ptr<SelectProcedureNode> site1 = nullptr,
                                std::shared_ptr<SelectProcedureNode> site2 = nullptr);
    ~CalculateAngleProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Whether the angle should be considered symmetric about 90 (i.e. 0 == 180)
    bool symmetric_{false};

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
