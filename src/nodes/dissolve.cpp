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
double DissolveGraph::pairPotentialRange() { return pairPotentialRange_; }

// Return first PairPotential in list
const std::vector<PairPotential::Definition> &DissolveGraph::pairPotentials()
{
    auto values = std::views::values(pairPotentialStore_.map());
    return {values.begin(), values.end()};
}

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

// Update pair potential store
void DissolveGraph::updatePairPotentials(const AtomType &i, const AtomType &j)
{
    auto nameI = i.name(), nameJ = j.name();
    if (pairPotentialStore_.contains(nameI, nameJ))
        return;

    pairPotentialStore_.set(
        nameI, nameJ,
        {std::make_shared<AtomType>(i), std::make_shared<AtomType>(j), std::make_unique<PairPotential>(nameI, nameJ)});
}