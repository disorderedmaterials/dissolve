// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/operatebase.h"

// Forward Declarations
class SelectProcedureNode;

// Operate Site Population Normalise Node
class OperateSitePopulationNormaliseProcedureNode : public OperateProcedureNodeBase
{
    public:
    OperateSitePopulationNormaliseProcedureNode();
    OperateSitePopulationNormaliseProcedureNode(RefList<const SelectProcedureNode> selectNodes);
    ~OperateSitePopulationNormaliseProcedureNode() override = default;

    /*
     * Target Sites (SelectNodes)
     */
    private:
    // List of sites (nodes) whose populations will be used to normalise the data
    RefList<const SelectProcedureNode> selectNodes_;

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
