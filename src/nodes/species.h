// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "classes/structure.h"
#include "nodes/forcefield.h"
#include "nodes/node.h"

// Species Node
class SpeciesNode : public Node
{
    public:
    SpeciesNode(Graph *parentGraph);
    ~SpeciesNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Species object
    Species species_;
    // Structure source for the species (optional)
    std::optional<Structure> structure_;
    // Forcefield recipe for the species (optional)
    std::optional<ForcefieldRecipe> recipe_;

    /*
     * Accessors
     */
    public:
    // Access the species information (Needed for SpeciesModel)
    Species &species();
    const Species &species() const;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    /*
     * Serialisation
     */
    private:
    // Serialise any hidden content
    void serialiseInternal(SerialisedValue &target) const override;
    // Deserialise any hidden content
    void deserialiseInternal(const SerialisedValue &target) override;
};
