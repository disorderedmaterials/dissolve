// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "main/dissolve.h"
#include "nodes/graph.h"

// Dissolve graph
class DissolveGraph : public Graph
{
    public:
    explicit DissolveNode(Dissolve& dissolve) : Node(this), dissolve_(dissolve) {};
    ~DissolveNode() override = default;

    public:
    std::string_view type() const override { return "Dissolve"; }
    std::string_view summary() const override { return "Parent node of all simulations"; }

    /*
     * Definition
     */
    private:
    // Dissolve reference
    Dissolve& dissolve_;

    public:
    // Return dissolve
    Dissolve &dissolve() const override;
    // Return world pool
    ProcessPool &processPool() const override;
};
