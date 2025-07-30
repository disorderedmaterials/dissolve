// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/dissolve.h"

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

const std::vector<std::shared_ptr<AtomType>> DissolveGraph::atomTypes(const Configuration *configuration)
{
    auto pop = configuration->atomTypePopulations();
    std::vector<std::shared_ptr<AtomType>> atomTypes;
    atomTypes.resize(pop.nItems());

    for (int i = 0; i < pop.nItems(); i++)
        atomTypes[i] = pop.atomType(i);

    return atomTypes;
}

bool DissolveGraph::updatePairPotentials(Dissolve &dissolve, const std::vector<std::shared_ptr<AtomType>> &atomTypes,
                                         std::optional<bool> useCombinationRulesHint)
{
    auto &potentialMap = dissolve.potentialMap();

    Messenger::print("Updating pair potentials...\n");
    potentialMap.clear();

    auto useCombinationRules = useCombinationRulesHint.value_or(dissolve.useCombinationRules());

    // Set the charge hadling for all pair potentials
    PairPotential::setIncludeCoulombPotential(dissolve.atomTypeChargeSource());

    // First step - remove any pair potentials which reference non-existent atom types
    auto &pairPotentials = dissolve.pairPotentials();
    pairPotentials.erase(
        std::remove_if(pairPotentials.begin(), pairPotentials.end(),
                       [&](const auto &pot)
                       {
                           return (std::find(atomTypes.begin(), atomTypes.end(), std::get<0>(pot)) == atomTypes.end() ||
                                   std::find(atomTypes.begin(), atomTypes.end(), std::get<1>(pot)) == atomTypes.end());
                       }),
        pairPotentials.end());

    // Second step - add or update tabulated pair potentials defined by the parameters and form of the associated atom types
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int typeI, const auto &at1, int typeJ, const auto &at2)
                            {
                                // Try to locate existing pair potential between these atom types
                                auto *pot = dissolve.pairPotential(at1, at2);

                                // If it doesn't exist we create it
                                if (!pot)
                                {
                                    Messenger::print("Creating new PairPotential for interaction between '{}' and '{}'...\n",
                                                     at1->name(), at2->name());
                                    pot = dissolve.addPairPotential(at1, at2);
                                }

                                // Update basic parameters
                                pot->setNames(at1->name(), at2->name());

                                // Auto-update parameters using combination rules?
                                if (useCombinationRules)
                                {
                                    // Combine the atom type parameters into potential function parameters
                                    auto optPotential =
                                        ShortRangeFunctions::combine(at1->interactionPotential(), at2->interactionPotential());
                                    if (optPotential)
                                        pot->setInteractionPotential(*optPotential);
                                    else
                                        pot->setInteractionPotential(Functions1D::Form::None, "");
                                }
                            });
    // Re-tabulate the potentials to account for changes in charge inclusion/exclusion, range etc. as well as parameters
    for (auto &&[at1, at2, pot] : pairPotentials)
    {
        pot->tabulate(dissolve.pairPotentialRange(), dissolve.pairPotentialDelta(), at1->charge() * at2->charge());
    }

    // Fourth step - set any additional potential
    for (auto &&[at1, at2, pp] : pairPotentials)
    {
        // Check processing module data for a named additional potential
        auto addPotName = std::format("Potential_{}-{}_Additional", at1->name(), at2->name());
        /*
        if (processingModuleData_.contains(addPotName, "Dissolve"))
            pp->setAdditionalPotential(processingModuleData_.retrieve<Data1D>(addPotName, "Dissolve"));
        */
    }

    return potentialMap.initialise(atomTypes, dissolve.pairPotentials(), dissolve.pairPotentialRange());
}