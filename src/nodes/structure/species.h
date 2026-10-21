// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/structure.h"
#include "nodes/forcefield.h"
#include "nodes/structure/speciesBase.h"

// Species Node
class SpeciesNode : public SpeciesBaseNode
{
    public:
    SpeciesNode(Graph *parentGraph);
    ~SpeciesNode() override = default;

    /*
     * Definition
     */
    public:
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Structure source for the species (optional)
    std::optional<Structure> structure_;
    // Forcefield recipe for the species (optional)
    std::optional<ForcefieldRecipe> recipe_;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
