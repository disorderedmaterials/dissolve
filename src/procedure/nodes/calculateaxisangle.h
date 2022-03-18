// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "procedure/nodes/calculatebase.h"

// Forward Declarations
class SelectProcedureNode;

// Calculate AxisAngle Node
class CalculateAxisAngleProcedureNode : public CalculateProcedureNodeBase
{
    public:
    CalculateAxisAngleProcedureNode(std::shared_ptr<SelectProcedureNode> site0 = nullptr,
                                    OrientedSite::SiteAxis axis0 = OrientedSite::XAxis,
                                    std::shared_ptr<SelectProcedureNode> site1 = nullptr,
                                    OrientedSite::SiteAxis axis1 = OrientedSite::XAxis);
    ~CalculateAxisAngleProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Axes to use for sites
    OrientedSite::SiteAxis axes_[2];

    public:
    // Return axis specified
    OrientedSite::SiteAxis &axis(int n);

    /*
     * Observable Target (implements virtuals in CalculateProcedureNodeBase)
     */
    public:
    // Return number of sites required to calculate observable
    int nSitesRequired() const override;
    // Return dimensionality of calculated observable
    int dimensionality() const override;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
