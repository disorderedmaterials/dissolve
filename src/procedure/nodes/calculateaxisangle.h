// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "procedure/nodes/calculatebase.h"
#include "templates/array.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate AxisAngle Node
class CalculateAxisAngleProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateAxisAngleProcedureNode(SelectProcedureNode *site0 = nullptr, OrientedSite::SiteAxis axis0 = OrientedSite::XAxis,
                                    SelectProcedureNode *site1 = nullptr, OrientedSite::SiteAxis axis1 = OrientedSite::XAxis);
    ~CalculateAxisAngleProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Axis to use from site 'I' (retrieved from keyword)
    int axisI_;
    // Axis to use from site 'J' (retrieved from keyword)
    int axisJ_;

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
