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

/*
 * Pair Potentials, Energy and Forces
 */

// Apply relevant overrides to the specified pair potential
void DissolveGraph::applyPairPotentialOverrides(PairPotential &pot)
{
    for (const auto &override : pairPotentialOverrides_)
    {
        // Is the override enabled?
        if (override->type() == PairPotentialOverride::PairPotentialOverrideType::Off)
            continue;

        // Is this override a match for the atom types in the potential?
        if ((DissolveSys::sameWildString(override->matchI(), pot.nameI()) &&
             DissolveSys::sameWildString(override->matchJ(), pot.nameJ())) ||
            (DissolveSys::sameWildString(override->matchJ(), pot.nameI()) &&
             DissolveSys::sameWildString(override->matchI(), pot.nameJ())))
        {
            // Create a function wrapper for the potential
            Function1DWrapper overridePotential(override->interactionPotential().form(),
                                                override->interactionPotential().parameters());
            // Apply the potential
            pot.addToReferenceShortRangePotential(
                overridePotential, override->type() == PairPotentialOverride::PairPotentialOverrideType::Replace);
        }
    }
}

// Update specified pair potential
void DissolveGraph::updatePairPotential(const AtomType &i, const AtomType &j)
{
    auto nameI = i.name(), nameJ = j.name();

    // Create the potential if it doesn't already exist
    if (!pairPotentialStore_.contains(nameI, nameJ))
    {
        auto interactionPotential = ShortRangeFunctions::combine(i.interactionPotential(), j.interactionPotential());

        if (interactionPotential.has_value())
            pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ, *interactionPotential});
        else
            pairPotentialStore_.set(nameI, nameJ, {nameI, nameJ});

        auto &pot = pairPotentialStore_.get({nameI, nameJ});
        pot.setLocalChargeProduct(i.charge() * j.charge());
    }

    // Update the tabulated potential
    auto &pot = pairPotentialStore_.get(nameI, nameJ);
    pot.tabulate();

    // Apply any relevant overrides
    applyPairPotentialOverrides(pot);
}

// Ensure that the specified Configuration has updated type indexing, cells etc.
void DissolveGraph::updateIndexingAndCells(Configuration *cfg) const
{
    // Update atom type indexing
    cfg->updateTypeIndexing();

    // Regenerate cells in the configuration if necessary
    cfg->updateCells();
}

// Create new pair potential override
void DissolveGraph::addPairPotentialOverride(std::string_view matchI, std::string_view matchJ,
                                             PairPotentialOverride::PairPotentialOverrideType overrideType,
                                             const InteractionPotential<Functions1D> &potential)
{
    pairPotentialOverrides_.emplace_back(std::make_unique<PairPotentialOverride>(matchI, matchJ, overrideType, potential));
}

// Return defined pair potential overrides
std::vector<std::unique_ptr<PairPotentialOverride>> &DissolveGraph::pairPotentialOverrides() { return pairPotentialOverrides_; }
const std::vector<std::unique_ptr<PairPotentialOverride>> &DissolveGraph::pairPotentialOverrides() const
{
    return pairPotentialOverrides_;
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

// Create a force kernel suitable for the supplied Configuration
std::unique_ptr<ForceKernel> DissolveGraph::createForceKernel(Configuration *cfg)
{
    // Update types and cells in Configuration
    updateIndexingAndCells(cfg);
    auto atomTypes = cfg->atomTypeVector();

    // Update pair potentials
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, const auto &atI, int j, const auto &atJ) { updatePairPotential(*atI, *atJ); });

    // Generate and return kernel
    return KernelProducer::forceKernel(cfg, PotentialMap(atomTypes, pairPotentialStore_));
}
