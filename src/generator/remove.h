// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "generator/node.h"
#include "generator/nodeValue.h"

// Forward Declarations
class Species;
class PickGeneratorNodeBase;

// Remove Node
class RemoveGeneratorNode : public GeneratorNode
{
    public:
    RemoveGeneratorNode();
    ~RemoveGeneratorNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Data
     */
    private:
    // Selection of molecules to remove
    std::shared_ptr<const PickGeneratorNodeBase> selection_;
    // Species to remove
    std::vector<const Species *> speciesToRemove_;

    /*
     * Execute
     */
    public:
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
