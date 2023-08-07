// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "module/layer.h"
#include "procedure/nodes/node.h"
#include "templates/flags.h"

class RunModuleListNode : public ProcedureNode
{
    public:
    explicit RunModuleListNode(std::vector<Module *> modules = {}, int frequency = 0,
                              const Flags<ModuleLayer::RunControlFlag>& flags = {});

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    private:
    // Target modules to run
    std::vector<Module *> modules_;
    // Frequency to run module list at
    int frequency_;
    bool energyStability_;
    bool sizeFactors_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};