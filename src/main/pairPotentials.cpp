// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "main/dissolve.h"
#include <utility>

// Set whether pair potentials are updated automatically through combination rules
void Dissolve::setUseCombinationRules(bool b) { useCombinationRules_ = b; }

// Return whether pair potentials are updated automatically through combination rules
bool Dissolve::useCombinationRules() const { return useCombinationRules_; }

// Return index of specified PairPotential
int Dissolve::indexOf(PairPotential *pp)
{
    auto result = std::find_if(pairPotentials_.begin(), pairPotentials_.end(),
                               [pp](const auto &ppDef) { return pp == std::get<2>(ppDef).get(); });

    return result == pairPotentials_.end() ? -1 : result - pairPotentials_.begin();
}

// Return number of defined PairPotentials
int Dissolve::nPairPotentials() const { return pairPotentials_.size(); }

// Add new pair potential to list
PairPotential *Dissolve::addPairPotential(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2)
{
    auto &&[atI, atJ, pp] = pairPotentials_.emplace_back(at1, at2, std::make_unique<PairPotential>(at1->name(), at2->name()));

    return pp.get();
}

// Return first PairPotential in list
const std::vector<PairPotential::Definition> &Dissolve::pairPotentials() const { return pairPotentials_; }

std::vector<PairPotential::Definition> &Dissolve::pairPotentials() { return pairPotentials_; }

// Return nth PairPotential in list
PairPotential *Dissolve::pairPotential(int n) { return std::get<2>(pairPotentials_[n]).get(); }

// Return specified PairPotential (if defined)
PairPotential *Dissolve::pairPotential(const AtomType *at1, const AtomType *at2) const
{
    auto it = std::find_if(pairPotentials_.begin(), pairPotentials_.end(),
                           [at1, at2](const auto &ppDef)
                           {
                               return (std::get<0>(ppDef).get() == at1 && std::get<1>(ppDef).get() == at2) ||
                                      (std::get<0>(ppDef).get() == at2 && std::get<1>(ppDef).get() == at1);
                           });
    return it != pairPotentials_.end() ? std::get<2>(*it).get() : nullptr;
}

PairPotential *Dissolve::pairPotential(std::string_view at1Name, std::string_view at2Name) const
{
    auto it = std::find_if(pairPotentials_.begin(), pairPotentials_.end(),
                           [at1Name, at2Name](const auto &ppDef)
                           {
                               return (DissolveSys::sameString(std::get<0>(ppDef)->name(), at1Name) &&
                                       DissolveSys::sameString(std::get<1>(ppDef)->name(), at2Name)) ||
                                      (DissolveSys::sameString(std::get<0>(ppDef)->name(), at2Name) &&
                                       DissolveSys::sameString(std::get<1>(ppDef)->name(), at1Name));
                           });
    return it != pairPotentials_.end() ? std::get<2>(*it).get() : nullptr;
}

// Return map for PairPotentials
const PotentialMap &Dissolve::potentialMap() const { return potentialMap_; }
PotentialMap &Dissolve::potentialMap() { return potentialMap_; }

// Update all pair potentials
bool Dissolve::updatePairPotentials(std::optional<bool> useCombinationRulesHint)
{
    Messenger::print("Updating pair potentials...\n");
    potentialMap_.clear();

    auto useCombinationRules = useCombinationRulesHint.value_or(useCombinationRules_);

    // First step - remove any pair potentials which reference non-existent atom types
    pairPotentials_.erase(std::remove_if(pairPotentials_.begin(), pairPotentials_.end(),
                                         [&](const auto &pot)
                                         {
                                             return (std::find(coreData_.atomTypes().begin(), coreData_.atomTypes().end(),
                                                               std::get<0>(pot)) == coreData_.atomTypes().end() ||
                                                     std::find(coreData_.atomTypes().begin(), coreData_.atomTypes().end(),
                                                               std::get<1>(pot)) == coreData_.atomTypes().end());
                                         }),
                          pairPotentials_.end());

    // Second step - add or update tabulated pair potentials defined by the parameters and form of the associated atom types
    dissolve::for_each_pair(ParallelPolicies::seq, coreData_.atomTypes(),
                            [&](int typeI, const auto &at1, int typeJ, const auto &at2)
                            {
                                // Try to locate existing pair potential between these atom types
                                auto *pot = pairPotential(at1.get(), at2.get());

                                // If it doesn't exist we create it
                                if (!pot)
                                {
                                    Messenger::print("Creating new PairPotential for interaction between '{}' and '{}'...\n",
                                                     at1->name(), at2->name());
                                    pot = addPairPotential(at1, at2);
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

                                // Set local charge product
                                pot->setLocalChargeProduct(at1->charge() * at2->charge());
                            });

    // Re-tabulate the potentials to account for changes in charge inclusion/exclusion, range etc. as well as parameters
    for (auto &&[at1, at2, pot] : pairPotentials_)
        pot->tabulate();

    // Third step - apply any overrides
    Messenger::print("Applying pair potential overrides...\n");
    // REMOVED for Dissolve2

    // Fourth step - set any additional potential
    for (auto &&[at1, at2, pp] : pairPotentials_)
    {
        // Check processing module data for a named additional potential
        auto addPotName = std::format("Potential_{}-{}_Additional", at1->name(), at2->name());
        if (processingModuleData_.contains(addPotName, "Dissolve"))
            pp->setAdditionalPotential(processingModuleData_.retrieve<Data1D>(addPotName, "Dissolve"));
    }

    // Reinitialise the potential map
    return potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_);
}

// Clear additional potentials
void Dissolve::clearAdditionalPotentials()
{
    for (auto &&[at1, at2, pp] : pairPotentials_)
    {
        pp->resetAdditionalPotential();

        // Clear entry in processing module data if it exists
        auto itemName = std::format("Potential_{}-{}_Additional", at1->name(), at2->name());
        if (processingModuleData_.contains(itemName, "Dissolve"))
            processingModuleData_.remove(itemName, "Dissolve");
    }
}
