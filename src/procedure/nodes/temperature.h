// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once


#include "keywords/nodeValue.h"
#include "procedure/nodes/node.h"
class TemperatureProcedureNode : public ProcedureNode
{
    public:
    TemperatureProcedureNode();
    ~TemperatureProcedureNode() override = default;

    private:
    NodeValue temperature_{300.0};

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};

