// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include "templates/doubleKeyedMap.h"

// Forward declarations
class EnergyKernel;
class ForceKernel;
class PotentialMap;

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
    // Return the current iteration count
    int iteration() const override;
    // Access a pair potential
    PairPotential *pairPotential(const AtomType *at1, const AtomType *at2) const override;
    // Return the DissolveGraph reference
    DissolveGraph *dissolveGraph() override;

    /*
     * Pair Potentials, Energy and Forces
     */
    private:
    // Generated pair potentials
    DoubleKeyedMap<PairPotential> pairPotentials_{true};
    // Defined pair potential overrides
    std::vector<std::unique_ptr<PairPotentialOverride>> pairPotentialOverrides_;

    private:
    // Apply relevant overrides to the specified pair potential
    void applyPairPotentialOverrides(PairPotential &pot);
    // Update specified pair potential
    void updatePairPotential(const AtomType &i, const AtomType &j);

    public:
    // Clear all pair potentials
    void clearPairPotentials();
    // Ensure that the specified Configuration has updated type indexing, cells etc.
    void updateIndexingAndCells(Configuration *cfg) const;
    // Create new pair potential override
    PairPotentialOverride *addPairPotentialOverride(
        std::string_view matchI = "", std::string_view matchJ = "",
        PairPotentialOverride::PairPotentialOverrideType overrideType = PairPotentialOverride::PairPotentialOverrideType::Off,
        const InteractionPotential<Functions1D> &potential = {});
    // Return defined pair potential overrides
    std::vector<std::unique_ptr<PairPotentialOverride>> &pairPotentialOverrides();
    const std::vector<std::unique_ptr<PairPotentialOverride>> &pairPotentialOverrides() const;
    // Create an energy kernel suitable for the supplied Configuration
    std::unique_ptr<EnergyKernel> createEnergyKernel(Configuration *cfg);
    // Create a force kernel suitable for the supplied Configuration
    std::unique_ptr<ForceKernel> createForceKernel(Configuration *cfg);

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
