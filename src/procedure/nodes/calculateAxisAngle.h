// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "procedure/nodes/calculateBase.h"

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
     * Identity
     */
    public:
    // Set node name
    void setName(std::string_view name) override;

    /*
     * Parameters
     */
    private:
    // Defined parameters
    std::vector<std::shared_ptr<ExpressionVariable>> parameters_;
    // Pointers to individual parameters
    std::shared_ptr<ExpressionVariable> angleParameter_;

    public:
    // Return the named parameter (if it exists)
    std::shared_ptr<ExpressionVariable> getParameter(std::string_view name,
                                                     std::shared_ptr<ExpressionVariable> excludeParameter) override;
    // Return vector of all parameters for this node
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<ExpressionVariable>>> parameters() const override;

    /*
     * Data
     */
    private:
    // Axes to use for sites
    OrientedSite::SiteAxis axes_[2];
    // Whether the angle should be considered symmetric about 90 (i.e. 0 == 180)
    bool symmetric_{false};

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
