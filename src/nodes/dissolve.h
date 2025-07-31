// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/edge.h"
#include "nodes/graph.h"

// Main Dissolve Node
class DissolveGraph : public Graph
{
    public:
    DissolveGraph(Dissolve &dissolve);
    ~DissolveGraph() = default;

    /*
     * Definition (Virtuals)
     */
    public:
    // Return node name
    std::string_view name() const override;
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Dissolve reference
    Dissolve &dissolve_;

    public:
    // Return dissolve
    Dissolve &dissolve() const override;

    /*
     * Functions
     */
    public:
    // Add new atom type to atom types
    static const std::shared_ptr<AtomType> addAtomType(std::vector<std::shared_ptr<AtomType>> &atomTypes, Elements::Element Z);
    // Return atom types belonging to configuration
    static const std::vector<std::shared_ptr<AtomType>> atomTypes(const Configuration *configuration);
    // Update pair potentials
    static bool updatePairPotentials(Dissolve &dissolve, const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                                     std::optional<bool> useCombinationRulesHint = {});
};
