// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "generator/pickBase.h"
#include <memory>

// Forward Declarations
class Molecule;

// Pick by Proximity Node
class PickProximityGeneratorNode : public PickGeneratorNodeBase
{
    public:
    explicit PickProximityGeneratorNode();
    ~PickProximityGeneratorNode() override = default;

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
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
