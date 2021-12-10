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
    std::optional<double> minDistance_;
    // Maximum distance (Angstroms)
    std::optional<double> maxDistance_;
    // Minimum number to pick
    std::optional<int> minCount_;
    // Maximum number to pick
    std::optional<int> maxCount_;

    /*
     * Execute
     */
    public:
    // Execute node, targetting the supplied Configuration
    bool execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList) override;
};
