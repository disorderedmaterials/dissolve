// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/isotopologue.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "data/isotopes.h"

Isotopologue::Isotopologue() : ListItem<Isotopologue>(), parent_(nullptr) {}

/*
 * Basic Information
 */

// Set parent Species
void Isotopologue::setParent(const Species *parent) { parent_ = parent; }

// Return parent Species
const Species *Isotopologue::parent() const { return parent_; }

// Set name of Isotopologue
void Isotopologue::setName(std::string_view name) { name_ = name; }

// Return name of Isotopologue
std::string_view Isotopologue::name() const { return name_; }

/*
 * Isotope Definition
 */

// UpdateUpdate current AtomType/Isotopes against parent Species
void Isotopologue::update()
{
    // Prune any types in our list that are not used in the parent species
    const auto &usedAtomTypes = parent_->usedAtomTypes();
    isotopes_.erase(std::remove_if(isotopes_.begin(), isotopes_.end(),
                                   [&usedAtomTypes](auto value) { return !usedAtomTypes.contains(std::get<0>(value)); }),
                    isotopes_.end());

    // Add in any used atom types that are not currently in the list
    for (const auto &atd : usedAtomTypes)
    {
        auto it = std::find_if(isotopes_.begin(), isotopes_.end(),
                               [&atd](auto value) { return std::get<0>(value) == atd.atomType(); });
        if (it == isotopes_.end())
            isotopes_.emplace_back(atd.atomType(), Isotopes::naturalIsotope(atd.atomType()->Z()));
    }
}

// Validate current AtomType/Isotopes against available AtomTypes
void Isotopologue::checkAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes)
{
    for (const auto at : atomTypes)
        isotopes_.erase(
            std::remove_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; }),
            isotopes_.end());
}

// Set Isotope associated to AtomType
void Isotopologue::setAtomTypeIsotope(std::shared_ptr<AtomType> at, Isotope *isotope)
{
    // Check for NULL pointer
    if (!at)
    {
        // TODO Use assertion
        Messenger::error("NULL_POINTER - NULL AtomType pointer passed to Isotopologue::setAtomTypeIsotope().\n");
        return;
    }

    // Find the requested AtomType in the list
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    if (it == isotopes_.end())
        isotopes_.emplace_back(at, isotope);
    else
        std::get<1>(*it) = isotope;
}

// Return Isotope for specified AtomType
Isotope *Isotopologue::atomTypeIsotope(std::shared_ptr<AtomType> at) const
{
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    if (it == isotopes_.end())
        return Isotopes::naturalIsotope(at->Z());

    return std::get<1>(*it);
}

// Return AtomType/Isotope pairs list
const std::vector<std::tuple<std::shared_ptr<AtomType>, Isotope *>> &Isotopologue::isotopes() const { return isotopes_; }
