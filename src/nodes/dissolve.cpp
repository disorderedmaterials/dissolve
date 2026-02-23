// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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

// Update specified pair potential
void DissolveGraph::updatePairPotential(const AtomType &i, const AtomType &j)
{
    auto nameI = i.name(), nameJ = j.name();

    // Ensure existing potential is up-to-date if it exists
    if (pairPotentialStore_.contains(nameI, nameJ))
    {
        pairPotentialStore_.get(nameI, nameJ).tabulate();
        return;
    }

    auto interactionPotential = ShortRangeFunctions::combine(i.interactionPotential(), j.interactionPotential());

    if (interactionPotential.has_value())
        pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ, *interactionPotential});
    else
        pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ});

    auto &pot = pairPotentialStore_.get({nameI, nameJ});
    pot.setLocalChargeProduct(i.charge() * j.charge());
    pot.tabulate();
}

// Ensure that the specified Configuration has updated type indexing, cells etc.
void DissolveGraph::updateIndexingAndCells(Configuration *cfg) const
{
    // Update atom type indexing
    cfg->updateTypeIndexing();

    // Regenerate cells in the configuration if necessary
    cfg->updateCells();
}

// Create an energy kernel suitable for the supplied Configuration
std::unique_ptr<EnergyKernel> DissolveGraph::createEnergyKernel(Configuration *cfg)
{
    // Update types and cells in Configuration
    updateIndexingAndCells(cfg);
    auto atomTypes = cfg->atomTypeVector();

    // Update pair potentials
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, const auto &atI, int j, const auto &atJ) { updatePairPotential(*atI, *atJ); });

    // Generate and return kernel
    return KernelProducer::energyKernel(cfg, PotentialMap(atomTypes, pairPotentialStore_));
}
