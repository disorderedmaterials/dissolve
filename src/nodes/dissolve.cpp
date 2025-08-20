// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "kernels/producer.h"
#include "kernels/externalPotentials.h"
#include "kernels/energy.h"

DissolveGraph::DissolveGraph(Dissolve &dissolve) : Graph(nullptr), dissolve_(dissolve) {}

/*
 * Definitions (Virtuals)
 */

// Return node name
std::string_view DissolveGraph::name() const { return "Root"; }

// Return type of the node
std::string_view DissolveGraph::type() const { return "Dissolve"; }

// Return short summary of the node's purpose
std::string_view DissolveGraph::summary() const { return "Parent node of all simulations"; }

// Return dissolve
Dissolve &DissolveGraph::dissolve() const { return dissolve_; }

/*
 * Functions
 */

std::unique_ptr<EnergyKernel> DissolveGraph::prepareEnergyCalculation(Dissolve &dissolve,
                                                                      const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                                                                      Configuration *cfg, std::optional<double> energyCutoff)
{
	// Update atom type indexing
	cfg->updateTypeIndexing();

	// Generate configuration potential map
    PotentialMap potentialMap;
    potentialMap.initialise(atomTypes, dissolve.pairPotentials(), dissolve.pairPotentialRange());

	// Regenerate cells
    cfg->cells().generate(cfg->box(), cfg->requestedCellDivisionLength(), potentialMap.range());

	// Produce energy kernel
	if (!cfg->globalPotentials().empty() || !cfg->targetedPotentials().empty())
        return std::make_unique<EnergyKernel>(cfg, potentialMap, energyCutoff);
    else
        return std::make_unique<EnergyKernel>(cfg, potentialMap, energyCutoff);
}
