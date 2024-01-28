// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

// Forward declarations
class Task;

class RunTaskNode : public ProcedureNode
{
    public:
    explicit RunTaskNode(Task *task = nullptr);
    ~RunTaskNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    private:
    // Target task to run
    Task *task_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};