// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class Species;
class PickGeneratorNodeBase;

// Transmute Node
class TransmuteGeneratorNode : public GeneratorNode
{
    public:
    TransmuteGeneratorNode();
    ~TransmuteGeneratorNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Control
     */
    private:
    // Existing selection from which to pick
    std::shared_ptr<const PickGeneratorNodeBase> selection_;
    // Target species to transmute in to
    const Species *targetSpecies_{nullptr};
    // Species to transmute
    std::vector<const Species *> speciesToTransmute_;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
