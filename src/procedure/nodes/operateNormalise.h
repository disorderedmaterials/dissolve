// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/operateBase.h"

// Operate Value Normalise Node
class OperateNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    explicit OperateNormaliseProcedureNode(double value = 1.0);
    ~OperateNormaliseProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Operating value
    NodeValue value_{1.0};
    // Whether to normalise absolute sum of values rather than direct sum
    bool absolute_{true};

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
