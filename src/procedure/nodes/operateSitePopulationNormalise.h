// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operateBase.h"

// Forward Declarations
class SelectProcedureNode;

// Operate Site Population Normalise Node
class OperateSitePopulationNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateSitePopulationNormaliseProcedureNode(ConstNodeVector<SelectProcedureNode> sites = {});
    ~OperateSitePopulationNormaliseProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Select nodes containing sites for normalisation
    ConstNodeVector<SelectProcedureNode> normalisationSites_;

    /*
     * Data Target (implements virtuals in OperateProcedureNodeBase)
     */
    public:
    // Operate on Data1D target
    bool operateData1D(const ProcessPool &procPool, Configuration *cfg) override;
    // Operate on Data2D target
    bool operateData2D(const ProcessPool &procPool, Configuration *cfg) override;
    // Operate on Data3D target
    bool operateData3D(const ProcessPool &procPool, Configuration *cfg) override;
};
