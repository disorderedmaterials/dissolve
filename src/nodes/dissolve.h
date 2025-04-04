// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "main/dissolve.h"
#include "nodes/graph.h"

// Main Dissolve Node
class DissolveGraph : public Graph
{
    public:
    DissolveGraph(Dissolve &dissolve);
    ~DissolveGraph() = default;

    public:
    std::string_view type() const override { return "Dissolve"; }
    std::string_view summary() const override { return "Parent node of all simulations"; }

    /*
     * Definition
     */
    private:
    // Dissolve reference
    Dissolve &dissolve_;

    public:
    // Return dissolve
    Dissolve &dissolve() const override;
    // Return world pool
    const ProcessPool &processPool() const override;
};
