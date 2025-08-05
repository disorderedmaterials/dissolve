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
    // Potential map cache
    inline static std::map<const Configuration *, std::unique_ptr<PotentialMap>> potentialMapCache_;

    public:
    // Return dissolve
    Dissolve &dissolve() const override;

    /*
     * Functions
     */
    public:
    // Return potential map based on configuration atom types
    static PotentialMap &potentialMap(const Configuration *configuration);
};
