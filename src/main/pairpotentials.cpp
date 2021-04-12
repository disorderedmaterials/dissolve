// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

// Set whether Coulomb term should be included in generated PairPotentials
void Dissolve::setPairPotentialsIncludeCoulomb(bool b) { pairPotentialsIncludeCoulomb_ = b; }

// Return whether Coulomb term should be included in generated PairPotentials
bool Dissolve::pairPotentialsIncludeCoulomb() { return pairPotentialsIncludeCoulomb_; }

// Return index of specified PairPotential
int Dissolve::indexOf(PairPotential *pp) { return pairPotentials_.indexOf(pp); }

// Return number of defined PairPotentials
int Dissolve::nPairPotentials() const { return pairPotentials_.nItems(); }

// Add new pair potential to list
PairPotential *Dissolve::addPairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2)
{
    PairPotential *pp = pairPotentials_.add();
    pp->setUp(at1, at2);

    return pp;
}

// Return first PairPotential in list
const List<PairPotential> &Dissolve::pairPotentials() const { return pairPotentials_; }

// Return nth PairPotential in list
PairPotential *Dissolve::pairPotential(int n) { return pairPotentials_[n]; }

// Return whether specified PairPotential is defined
PairPotential *Dissolve::pairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2) const
{
    for (auto *pot = pairPotentials_.first(); pot != nullptr; pot = pot->next())
    {
        if ((pot->atomTypeI() == at1) && (pot->atomTypeJ() == at2))
            return pot;
        if ((pot->atomTypeI() == at2) && (pot->atomTypeJ() == at1))
            return pot;
    }
    return nullptr;
}

// Return whether specified PairPotential is defined
PairPotential *Dissolve::pairPotential(std::string_view at1, std::string_view at2) const
{
    for (auto *pot = pairPotentials_.first(); pot != nullptr; pot = pot->next())
    {
        if (DissolveSys::sameString(pot->atomTypeNameI(), at1) && DissolveSys::sameString(pot->atomTypeNameJ(), at2))
            return pot;
        if (DissolveSys::sameString(pot->atomTypeNameI(), at2) && DissolveSys::sameString(pot->atomTypeNameJ(), at1))
            return pot;
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
bool Dissolve::generatePairPotentials(std::shared_ptr<AtomType> onlyInvolving)
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
            if (pot->shortRangeType() == Forcefield::UndefinedType)
                ++nUndefined;
            else if (!pot->tabulate(pairPotentialRange_, pairPotentialDelta_, pairPotentialsIncludeCoulomb_))
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
