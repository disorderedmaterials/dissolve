// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include "templates/doubleKeyedMap.h"

class EnergyKernel;
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
    // Pair potential store
    DoubleKeyedMap<std::shared_ptr<PairPotential>> pairPotentialStore_{true};
    // Pair potential range
    double pairPotentialRange_{12};

    public:
    // Return dissolve
    Dissolve &dissolve() const;
    // Return the DissolveGraph reference
    DissolveGraph *dissolveGraph() override;
    // Return pair potential store
    const DoubleKeyedMap<std::shared_ptr<PairPotential>> &pairPotentialStore();

    /*
     * Functions
     */
    public:
    // Return maximum distance for tabulated PairPotentials
    const double pairPotentialRange() const;
    // Return energy kernel containing potential map
    std::unique_ptr<EnergyKernel> prepareEnergyCalculation(Configuration *cfg, std::optional<double> energyCutoff = {});

    private:
    // Update pair potential store
    void updatePairPotentials(const AtomType &i, const AtomType &j);
};
