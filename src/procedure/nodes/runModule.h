// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
class Module;

class RunModuleNode : public ProcedureNode
{
    public:
    explicit RunModuleNode(Module *module = nullptr);
    ~RunModuleNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    private:
    // Target module to run
    Module *module_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};