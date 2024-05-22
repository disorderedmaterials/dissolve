// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include <utility>

#include "classes/atomType.h"
#include "main/dissolve.h"

// Set maximum distance for tabulated PairPotentials
void Dissolve::setPairPotentialRange(double range) { pairPotentialRange_ = range; }

// Return maximum distance for tabulated PairPotentials
double Dissolve::pairPotentialRange() const { return pairPotentialRange_; }

// Set delta to use in tabulations
void Dissolve::setPairPotentialDelta(double delta) { pairPotentialDelta_ = delta; }

// Return delta to use in tabulations
double Dissolve::pairPotentialDelta() const { return pairPotentialDelta_; }

// Set whether to automatically determine charge source
void Dissolve::setAutomaticChargeSource(bool b) { automaticChargeSource_ = b; };

// Return whether to automatically determine charge source
bool Dissolve::automaticChargeSource() const { return automaticChargeSource_; }

// Set whether to force the use of the specified charge source (if not automatic choice)
void Dissolve::setForceChargeSource(bool b) { forceChargeSource_ = b; }

// Return whether to force the use of the specified charge source (if not automatic choice)
bool Dissolve::forceChargeSource() const { return forceChargeSource_; }

// Set whether charges from atom types are to be used (and included in PairPotentials)
void Dissolve::setAtomTypeChargeSource(bool b) { atomTypeChargeSource_ = b; }

// Return whether charges from atom types are to be used (and included in PairPotentials)
bool Dissolve::atomTypeChargeSource() const { return atomTypeChargeSource_; }

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

// Return nth PairPotential in list
PairPotential *Dissolve::pairPotential(int n) { return std::get<2>(pairPotentials_[n]).get(); }

// Return specified PairPotential (if defined)
PairPotential *Dissolve::pairPotential(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2) const
{
    auto it = std::find_if(pairPotentials_.begin(), pairPotentials_.end(),
                           [at1, at2](const auto &ppDef)
                           {
                               return (std::get<0>(ppDef) == at1 && std::get<1>(ppDef) == at2) ||
                                      (std::get<0>(ppDef) == at2 && std::get<1>(ppDef) == at1);
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

// Update all pair potentials
bool Dissolve::updatePairPotentials(std::optional<bool> useCombinationRulesHint)
{
    Messenger::print("Updating pair potentials...\n");
    potentialMap_.clear();

    auto useCombinationRules = useCombinationRulesHint.value_or(useCombinationRules_);

    // First step - add or update tabulated pair potentials defined by the parameters and form of the associated atom types
    if (!for_each_pair_early(coreData_.atomTypes().begin(), coreData_.atomTypes().end(),
                             [&](int typeI, const auto &at1, int typeJ, const auto &at2) -> EarlyReturn<bool>
                             {
                                 // Try to locate existing pair potential between these atom types
                                 auto *pot = pairPotential(at1, at2);

                                 // If it doesn't exist we create it
                                 if (!pot)
                                 {
                                     Messenger::print("Creating new PairPotential for interaction between '{}' and '{}'...\n",
                                                      at1->name(), at2->name());
                                     pot = addPairPotential(at1, at2);
                                 }

                                 // Set / update basic parameters
                                 pot->setNames(at1->name(), at2->name());
                                 if (atomTypeChargeSource_)
                                     pot->setIncludedCharges(at1->charge(), at2->charge());
                                 else
                                     pot->setNoIncludedCharges();

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

                                 return EarlyReturn<bool>::Continue;
                             })
             .value_or(true))
        return false;

    // Re-tabulate the potentials to account for changes in charge inclusion/exclusion, range etc. as well as parameters
    auto tabulationSucceeded = true;
    for (auto &&[at1, at2, pot] : pairPotentials_)
    {
        if (!pot->tabulate(pairPotentialRange_, pairPotentialDelta_))
            tabulationSucceeded = false;
    }
    if (!tabulationSucceeded)
        return false;

    // Second step - apply any overrides
    Messenger::print("Applying pair potential overrides...\n");
    for (const auto &override : coreData_.pairPotentialOverrides())
    {
        Messenger::print("Pair potential override between '{}' and '{}' ({}, {}, '{}') ...\n", override->matchI(),
                         override->matchJ(), PairPotentialOverride::pairPotentialOverrideTypes().keyword(override->type()),
                         Functions1D::forms().keyword(override->interactionPotential().form()),
                         override->interactionPotential().parametersAsString());

        // Is the override enabled?
        if (override->type() == PairPotentialOverride::PairPotentialOverrideType::Off)
        {
            Messenger::print(" ... is currently 'Off'.\n");
            continue;
        }

        // Generate the potential
        PairPotential overridePotential(override->matchI(), override->matchJ(), override->interactionPotential());
        if (!overridePotential.tabulate(pairPotentialRange_, pairPotentialDelta_))
            return false;

        auto count = 0;
        for (auto &&[at1, at2, pp] : pairPotentials_)
        {
            // Is this override a match for the atom types in the potential?
            if ((DissolveSys::sameWildString(override->matchI(), at1->name()) &&
                 DissolveSys::sameWildString(override->matchJ(), at2->name())) ||
                (DissolveSys::sameWildString(override->matchJ(), at1->name()) &&
                 DissolveSys::sameWildString(override->matchI(), at2->name())))
            {
                Messenger::print(" ... matched and was applied to defined potential {}-{}\n", at1->name(), at2->name());

                // Apply the potential
                switch (override->type())
                {
                    case (PairPotentialOverride::PairPotentialOverrideType::Off):
                        break;
                    case (PairPotentialOverride::PairPotentialOverrideType::Add):
                        pp->uOriginal() += overridePotential.uOriginal();
                        break;
                    case (PairPotentialOverride::PairPotentialOverrideType::Replace):
                        pp->uOriginal() = overridePotential.uOriginal();
                        break;
                }

                pp->calculateUFull();
                pp->calculateDUFull();

                ++count;
            }
        }
        Messenger::print(" ... matched {} potential(s) in total.\n", count);
    }

    // Lastly, set any additional potential
    for (auto &&[at1, at2, pp] : pairPotentials_)
    {
        // Check processing module data for a named additional potential
        auto addPotName = fmt::format("Potential_{}-{}_Additional", at1->name(), at2->name());
        if (processingModuleData_.contains(addPotName, "Dissolve"))
            pp->setUAdditional(processingModuleData_.retrieve<Data1D>(addPotName, "Dissolve"));
    }

    // Reinitialise the potential map
    return potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_, pairPotentialRange_);
}

// Revert potentials to reference state, clearing additional potentials
void Dissolve::revertPairPotentials()
{
    for (auto &&[at1, at2, pp] : pairPotentials_)
    {
        pp->resetUAdditional();

        // Clear entry in processing module data if it exists
        auto itemName = fmt::format("Potential_{}-{}_Additional", at1->name(), at2->name());
        if (processingModuleData_.contains(itemName, "Dissolve"))
            processingModuleData_.remove(itemName, "Dissolve");
    }
}
