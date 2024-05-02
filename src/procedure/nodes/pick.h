// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/pickBase.h"
#include <memory>

// Forward Declarations
class Molecule;

// Pick Node
class PickProcedureNode : public PickProcedureNodeBase
{
    public:
    explicit PickProcedureNode(std::vector<const Species *> species = {});
    ~PickProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Species to pick
    std::vector<const Species *> speciesToPick_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
