// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodeValue.h"
#include "procedure/nodes/operateBase.h"

// Operate Multiply Node
class OperateMultiplyProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateMultiplyProcedureNode();
    ~OperateMultiplyProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Operating value
    NodeValue value_{1.0};

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
