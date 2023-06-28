// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/site.h"
#include "procedure/nodes/node.h"
class RotateProcedureNode : public ProcedureNode
{
    public:
    RotateProcedureNode(std::shared_ptr<SelectProcedureNode> site = nullptr);
    ~RotateProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Site to be rotated
    std::shared_ptr<const SelectProcedureNode> site_;
    // Rotation
    NodeValue rotation_;
    // Axis
    OrientedSite::SiteAxis axis_{OrientedSite::SiteAxis::XAxis};

    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
