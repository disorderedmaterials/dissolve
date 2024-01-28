// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"

// Forward Declarations
class Configuration;

class GenerateProcedureNode : public ProcedureNode
{
    public:
    explicit GenerateProcedureNode(Configuration *cfg = nullptr);

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Target
     */
    private:
    // Target configuration to generate
    Configuration *target_{nullptr};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};