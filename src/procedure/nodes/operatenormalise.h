// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operatebase.h"

// Forward Declarations
/* none */

// Operate Value Normalise Node
class OperateNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateNormaliseProcedureNode(double value = 1.0);
    OperateNormaliseProcedureNode(int value);
    ~OperateNormaliseProcedureNode() override = default;

    /*
     * Data Target (implements virtuals in OperateProcedureNodeBase)
     */
    public:
    // Operate on Data1D target
    bool operateData1D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data2D target
    bool operateData2D(ProcessPool &procPool, Configuration *cfg);
    // Operate on Data3D target
    bool operateData3D(ProcessPool &procPool, Configuration *cfg);
};
