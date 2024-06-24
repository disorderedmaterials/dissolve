// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
class SizeFactorProcedureNode : public ProcedureNode
{
    public:
    SizeFactorProcedureNode();
    ~SizeFactorProcedureNode() override = default;

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
