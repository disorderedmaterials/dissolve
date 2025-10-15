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

// Return the DissolveGraph reference
DissolveGraph *DissolveGraph::dissolveGraph() { return this; }

// Return pair potential store
const DoubleKeyedMap<PairPotential> &DissolveGraph::pairPotentialStore() { return pairPotentialStore_; }

/*
 * Functions
 */

// Return maximum distance for tabulated PairPotentials
const double DissolveGraph::pairPotentialRange() const { return pairPotentialRange_; }

// Return energy kernel containing potential map
std::unique_ptr<EnergyKernel> DissolveGraph::prepareEnergyCalculation(Configuration *cfg, std::optional<double> energyCutoff)
{
    auto atomTypes = cfg->atomTypeVector();

    // Update atom type indexing
    cfg->updateTypeIndexing();

    // Update pair potentials
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, const auto &atI, int j, const auto &atJ) { updatePairPotentials(*atI, *atJ); });

    // Generate configuration potential map
    PotentialMap potentialMap(atomTypes, pairPotentialStore(), pairPotentialRange());

    // Regenerate cells
    cfg->cells().generate(cfg->box(), cfg->requestedCellDivisionLength(), potentialMap.range());

    auto kernel = KernelProducer::energyKernel(cfg, potentialMap, energyCutoff);

    cfg->updateCells(kernel.get()->potentialMap().range());

    return kernel;
}

// Update pair potential store
void DissolveGraph::updatePairPotentials(const AtomType &i, const AtomType &j)
{
    auto nameI = i.name(), nameJ = j.name();
    if (pairPotentialStore_.contains(nameI, nameJ))
        return;

    auto interactionPotential = ShortRangeFunctions::combine(i.interactionPotential(), j.interactionPotential());

    if (interactionPotential.has_value())
        pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ, *interactionPotential});
    else
        pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ});

    auto &pot = pairPotentialStore_.get({nameI, nameJ});
    pot.tabulate(pairPotentialRange_, pairPotentialDelta_, i.charge() * j.charge());
}