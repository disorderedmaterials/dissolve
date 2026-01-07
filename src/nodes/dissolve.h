// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/edge.h"
#include "nodes/graph.h"
#include "templates/doubleKeyedMap.h"

// Forward declarations

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
    DoubleKeyedMap<PairPotential> pairPotentialStore_{true};
    // Pair potential range
    double pairPotentialRange_{12};
    // Pair potential delta
    double pairPotentialDelta_{0.005};

    public:
    // Return dissolve
    Dissolve &dissolve() const;
    // Return the DissolveGraph reference
    DissolveGraph *dissolveGraph() override;
    // Return pair potential store
    const DoubleKeyedMap<PairPotential> &pairPotentialStore();

    /*
     * Functions
     */
    public:
    // Return maximum distance for tabulated PairPotentials
    const double pairPotentialRange() const;
    // Return suitable energy kernel for the supplied Configuration
    std::unique_ptr<EnergyKernel> prepareEnergyCalculation(Configuration *cfg);

    private:
    // Update pair potential store
    void updatePairPotentials(const AtomType &i, const AtomType &j);
};
