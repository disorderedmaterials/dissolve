// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
class SizeFactorProcedureNode : public ProcedureNode
{
    public:
    SizeFactorProcedureNode();
    ~SizeFactorProcedureNode() override = default;

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
    NodeValue sizeFactor_{10.0};

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
