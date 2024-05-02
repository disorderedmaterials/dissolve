// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operateBase.h"
#include "procedure/nodes/select.h"

// Operate Number Density Normalise Node
class OperateNumberDensityNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateNumberDensityNormaliseProcedureNode(
        ConstNodeVector<SelectProcedureNode> nodes = {},
        SelectProcedureNode::SelectionPopulation populationTarget = SelectProcedureNode::SelectionPopulation::Available);
    ~OperateNumberDensityNormaliseProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Select nodes containing sites for normalisation
    ConstNodeVector<SelectProcedureNode> normalisationSites_;
    // Target population to normalise against
    SelectProcedureNode::SelectionPopulation targetPopulation_;

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
