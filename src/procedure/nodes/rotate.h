// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

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
    // Origin to rotate to.
    Vec3<NodeValue> origin_;
    
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
