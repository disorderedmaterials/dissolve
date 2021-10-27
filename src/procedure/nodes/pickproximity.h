// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "procedure/nodes/node.h"
#include "procedure/nodes/pickbase.h"
#include <memory>

// Forward Declarations
class Molecule;

// Pick by Proximity Node
class PickProximityProcedureNode : public PickProcedureNodeBase
{
    public:
    explicit PickProximityProcedureNode();
    ~PickProximityProcedureNode() override = default;

    /*
     * Control
     */
    private:
    // Species to pick
    std::vector<const Species *> speciesToPick_;
    // Minimum distance (Angstroms)
    double minDistance_{0.0};
    // Maximum distance (Angstroms)
    double maxDistance_{0.0};
    // Minimum number to pick
    int minCount_{0};
    // Maximum number to pick
    int maxCount_{0};

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
