// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operatebase.h"

// Forward Declarations
/* none */

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
    bool operateData1D(ProcessPool &procPool, Configuration *cfg);
};
