// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

class RunModuleListNode : public ProcedureNode
{
    public:
    explicit RunModuleListNode(std::vector<Module*> modules = {}, int frequency = 0);

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    private:
    // Target modules to run
    std::vector<Module*> modules_;
    // Frequency to run module list at
    int frequency_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};