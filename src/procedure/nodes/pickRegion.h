// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/pickBase.h"

// Forward Declarations
class RegionProcedureNodeBase;

// Pick Region Node
class PickRegionProcedureNode : public PickProcedureNodeBase
{
    public:
    explicit PickRegionProcedureNode(std::shared_ptr<const RegionProcedureNodeBase> region = nullptr);
    ~PickRegionProcedureNode() override = default;

    /*
     * Data
     */
    private:
    // Target region for the pick
    std::shared_ptr<const RegionProcedureNodeBase> region_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
