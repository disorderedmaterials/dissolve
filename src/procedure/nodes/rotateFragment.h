// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "procedure/nodes/node.h"

class RotateFragmentProcedureNode : public ProcedureNode
{
    public:
    RotateFragmentProcedureNode(std::shared_ptr<SelectProcedureNode> site = nullptr);
    ~RotateFragmentProcedureNode() override = default;

    /*
     * Identity
     */
    private:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Control
     */
    private:
    // Site to be rotated
    std::shared_ptr<const SelectProcedureNode> site_;
    // Rotation
    NodeValue rotation_{90.0};
    // Axis
    OrientedSite::SiteAxis axis_{OrientedSite::SiteAxis::XAxis};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
