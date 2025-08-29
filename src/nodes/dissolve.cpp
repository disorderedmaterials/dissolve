// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "kernels/producer.h"

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

// Return maximum distance for tabulated PairPotentials
double Dissolve::pairPotentialRange() { return pairPotentialRange_; }

// Return first PairPotential in list
std::vector<PairPotential::Definition> &Dissolve::pairPotentials() { return pairPotentials_; }

// Return energy kernel containing potential map
std::unique_ptr<EnergyKernel> DissolveGraph::prepareEnergyCalculation(Configuration *cfg, std::optional<double> energyCutoff)
{
	// Update atom type indexing
	cfg->updateTypeIndexing();

	// Generate configuration potential map
    PotentialMap potentialMap;
    auto atomTypeKeys = std::views::keys(cfg->atomTypeIndexMap());
    std::vector<const AtomType *> atomTypes{atomTypeKeys.begin(), atomTypeKeys.end()};
    potentialMap.initialise(atomTypes, pairPotentials(), pairPotentialRange());

	// Regenerate cells
    cfg->cells().generate(cfg->box(), cfg->requestedCellDivisionLength(), potentialMap.range());

    return KernelProducer::energyKernel(cfg, potentialMap, energyCutoff);
}
