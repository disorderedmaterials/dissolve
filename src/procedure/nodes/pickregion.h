// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/pickbase.h"

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
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
