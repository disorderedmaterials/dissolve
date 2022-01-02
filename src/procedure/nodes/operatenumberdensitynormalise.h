// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operatebase.h"

// Forward Declarations
class SelectProcedureNode;

// Operate Number Density Normalise Node
class OperateNumberDensityNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateNumberDensityNormaliseProcedureNode(std::vector<std::shared_ptr<const SelectProcedureNode>> nodes = {});
    ~OperateNumberDensityNormaliseProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Select nodes containing sites for normalisation
    std::vector<std::shared_ptr<const SelectProcedureNode>> normalisationSites_;

    /*
     * Data Target (implements virtuals in OperateProcedureNodeBase)
     */
    public:
    // Operate on Data1D target
    bool operateData1D(ProcessPool &procPool, Configuration *cfg) override;
    // Operate on Data2D target
    bool operateData2D(ProcessPool &procPool, Configuration *cfg) override;
    // Operate on Data3D target
    bool operateData3D(ProcessPool &procPool, Configuration *cfg) override;
};
