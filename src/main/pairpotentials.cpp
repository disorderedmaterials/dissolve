// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include <utility>

#include "classes/atomtype.h"
#include "main/dissolve.h"

// Set maximum distance for tabulated PairPotentials
void Dissolve::setPairPotentialRange(double range)
{
    pairPotentialRange_ = range;
    pairPotentialRangeSquared_ = range * range;
}

// Return maximum distance for tabulated PairPotentials
double Dissolve::pairPotentialRange() const { return pairPotentialRange_; }

// Return maximum squared distance for tabulated PairPotentials
double Dissolve::pairPotentialRangeSquared() const { return pairPotentialRangeSquared_; }

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
    auto result = std::find_if(pairPotentials_.begin(), pairPotentials_.end(), [pp](auto &p) { return pp == p.get(); });

    return result == pairPotentials_.end() ? -1 : result - pairPotentials_.begin();
}

// Return number of defined PairPotentials
int Dissolve::nPairPotentials() const { return pairPotentials_.size(); }

// Add new pair potential to list
PairPotential *Dissolve::addPairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2)
{
    auto &pp = pairPotentials_.emplace_back(std::make_unique<PairPotential>());
    pp->setUp(std::move(at1), std::move(at2));

    return pp.get();
}

// Return first PairPotential in list
const std::vector<std::unique_ptr<PairPotential>> &Dissolve::pairPotentials() const { return pairPotentials_; }

// Return nth PairPotential in list
PairPotential *Dissolve::pairPotential(int n) { return pairPotentials_[n].get(); }

// Return whether specified PairPotential is defined
PairPotential *Dissolve::pairPotential(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2) const
{
    for (auto &pot : pairPotentials_)
    {
        if ((pot->atomTypeI() == at1) && (pot->atomTypeJ() == at2))
            return pot.get();
        if ((pot->atomTypeI() == at2) && (pot->atomTypeJ() == at1))
            return pot.get();
    }
    return nullptr;
}

// Return whether specified PairPotential is defined
PairPotential *Dissolve::pairPotential(std::string_view at1, std::string_view at2) const
{
    for (auto &pot : pairPotentials_)
    {
        if (DissolveSys::sameString(pot->atomTypeNameI(), at1) && DissolveSys::sameString(pot->atomTypeNameJ(), at2))
            return pot.get();
        if (DissolveSys::sameString(pot->atomTypeNameI(), at2) && DissolveSys::sameString(pot->atomTypeNameJ(), at1))
            return pot.get();
    }
    return nullptr;
}

// Return map for PairPotentials
const PotentialMap &Dissolve::potentialMap() { return potentialMap_; }

// Clear and regenerate all PairPotentials, replacing those currently defined
void Dissolve::regeneratePairPotentials()
{
    potentialMap_.clear();
    pairPotentials_.clear();
    pairPotentialAtomTypeVersion_ = -1;

    generatePairPotentials();
}

// Generate all necessary PairPotentials, adding missing terms where necessary
bool Dissolve::generatePairPotentials(const std::shared_ptr<AtomType> &onlyInvolving)
{
    // Check current AtomTypes version against the last one we generated at
    if (pairPotentialAtomTypeVersion_ == coreData_.atomTypesVersion())
    {
        Messenger::printVerbose("PairPotentials are up to date with AtomTypes, so nothing to do.\n");
        return true;
    }

    auto nUndefined = 0;

    // Loop over all atomtype pairs and update / add pair potentials as necessary
    for (auto at1 = coreData_.atomTypes().begin(); at1 != coreData_.atomTypes().end(); ++at1)
    {
        for (auto at2 = at1; at2 != coreData_.atomTypes().end(); ++at2)
        {
            // If an AtomType was supplied, only generate the pair potential if one of its AtomTypes matches
            if (onlyInvolving && (*at1 != onlyInvolving) && (*at2 != onlyInvolving))
                continue;

            // Does a PairPotential for this AtomType pair already exist?
            auto *pot = pairPotential(*at1, *at2);
            if (pot)
            {
                Messenger::print("Updating existing PairPotential for interaction between '{}' and '{}'...\n", (*at1)->name(),
                                 (*at2)->name());
                if (!pot->setUp(*at1, *at2))
                    return false;
            }
            else
            {
                Messenger::print("Adding new PairPotential for interaction between '{}' and '{}'...\n", (*at1)->name(),
                                 (*at2)->name());
                pot = addPairPotential(*at1, *at2);
            }

            // Check the implied short-range form of the potential
            if (pot->shortRangeType() == Forcefield::ShortRangeType::Undefined)
                ++nUndefined;
            else if (!pot->tabulate(pairPotentialRange_, pairPotentialDelta_, atomTypeChargeSource_))
                return false;

            // Retrieve additional potential from the processing module data, if present
            std::string itemName = fmt::format("Potential_{}-{}_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ());
            if (!processingModuleData_.contains(itemName, "Dissolve"))
                continue;
            pot->setUAdditional(processingModuleData_.retrieve<Data1D>(itemName, "Dissolve"));
        }
    }

    pairPotentialAtomTypeVersion_ = coreData_.atomTypesVersion();

    return (nUndefined == 0);
}
