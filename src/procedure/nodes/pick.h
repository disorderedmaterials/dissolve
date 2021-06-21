// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/pickbase.h"
#include "templates/array.h"
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
     * Pick Criteria
     */
    private:
    // Target species (retrieved from keyword)
    std::vector<const Species *> targetSpecies_;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
