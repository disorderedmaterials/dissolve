// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"
#include "nodes/edge.h"
#include "nodes/graph.h"

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

    public:
    // Return dissolve
    Dissolve &dissolve() const override;

    /*
     * Functions
     */
    public:
    std::unique_ptr<EnergyKernel> prepareEnergyCalculation(Dissolve &dissolve, Configuration *cfg, std::optional<double> energyCutoff = {});
};
