// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/calculatebase.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate Vector Node
class CalculateVectorProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateVectorProcedureNode(SelectProcedureNode *site0 = nullptr, SelectProcedureNode *site1 = nullptr,
                                 bool rotateIntoFrame = false);
    ~CalculateVectorProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Whether to rotate into the frame of the first site (retrieved from keyword)
    bool rotateIntoFrame_;

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    public:
    // Return number of sites required to calculate observable
    int nSitesRequired() const;
    // Return dimensionality of calculated observable
    int dimensionality() const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
