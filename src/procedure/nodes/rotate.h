// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
class RotateNode : public ProcedureNode
{
    public:
    RotateNode();
    ~RotateNode() override = default;

    /*
     * Control
     */ 
    private:
    // Origin to rotate to.
    Vec3<NodeValue> origin_;
    
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
