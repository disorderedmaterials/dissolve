// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "main/dissolve.h"
#include <utility>

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
