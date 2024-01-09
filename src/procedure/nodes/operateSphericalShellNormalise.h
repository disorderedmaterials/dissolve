// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operateBase.h"

// Operate Spherical Shell Normalise Node
class OperateSphericalShellNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateSphericalShellNormaliseProcedureNode();
    ~OperateSphericalShellNormaliseProcedureNode() override = default;

    /*
     * Data Target (implements virtuals in OperateProcedureNodeBase)
     */
    public:
    // Operate on Data1D target
    bool operateData1D(const ProcessPool &procPool, Configuration *cfg) override;
    // Operate on Data2D target
    bool operateData2D(const ProcessPool &procPool, Configuration *cfg) override;
};
