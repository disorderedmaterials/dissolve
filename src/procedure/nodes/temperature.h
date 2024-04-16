// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/nodeValue.h"
#include "procedure/nodes/node.h"

class TemperatureProcedureNode : public ProcedureNode
{
    public:
    TemperatureProcedureNode();
    ~TemperatureProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Node Data
     */
    private:
    // Temperature for configuration
    NodeValue temperature_{300.0};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};