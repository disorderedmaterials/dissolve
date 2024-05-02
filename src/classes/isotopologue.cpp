// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopologue.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include "data/isotopes.h"

Isotopologue::Isotopologue(std::string name) : name_(name) {}

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
    const auto &atomTypes = parent_->atomTypes();
    isotopes_.erase(std::remove_if(isotopes_.begin(), isotopes_.end(),
                                   [&atomTypes](auto value) { return !atomTypes.contains(std::get<0>(value)); }),
                    isotopes_.end());

    // Add in any used atom types that are not currently in the list
    for (const auto &atd : atomTypes)
    {
        auto it = std::find_if(isotopes_.begin(), isotopes_.end(),
                               [&atd](auto value) { return std::get<0>(value) == atd.atomType(); });
        if (it == isotopes_.end())
            isotopes_.emplace_back(atd.atomType(), Sears91::naturalIsotope(atd.atomType()->Z()));
    }
}

// Validate current AtomType/Isotopes against available AtomTypes
void Isotopologue::checkAtomTypes(const std::vector<std::shared_ptr<AtomType>> &atomTypes)
{
    for (const auto &at : atomTypes)
        isotopes_.erase(
            std::remove_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; }),
            isotopes_.end());
}

// Set Isotope associated to AtomType
void Isotopologue::setAtomTypeIsotope(std::shared_ptr<AtomType> at, Sears91::Isotope isotope)
{
    assert(at);

    // Find the requested AtomType in the list
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    if (it == isotopes_.end())
        isotopes_.emplace_back(at, isotope);
    else
        std::get<1>(*it) = isotope;
}

// Return Isotope for specified AtomType
Sears91::Isotope Isotopologue::atomTypeIsotope(std::shared_ptr<AtomType> at) const
{
    auto it = std::find_if(isotopes_.begin(), isotopes_.end(), [&at](auto value) { return std::get<0>(value) == at; });
    if (it == isotopes_.end())
        return Sears91::naturalIsotope(at->Z());

    return std::get<1>(*it);
}

// Return AtomType/Isotope pairs list
std::vector<std::tuple<std::shared_ptr<AtomType>, Sears91::Isotope>> &Isotopologue::isotopes() { return isotopes_; }
const std::vector<std::tuple<std::shared_ptr<AtomType>, Sears91::Isotope>> &Isotopologue::isotopes() const { return isotopes_; }

// Express as a serialisable value
SerialisedValue Isotopologue::serialise() const
{
    SerialisedValue::table_type result;
    for (auto &&[type, isotope] : isotopes_)
        result[type->name().data()] = Sears91::A(isotope);
    return result;
}

void Isotopologue::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto &[name, value] : node.as_table())
    {
        if (value.is_string())
            continue;
        auto at = coreData.findAtomType(name);
        setAtomTypeIsotope(at, Sears91::isotope(at->Z(), value.as_integer()));
    }
}
