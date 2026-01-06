// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
    const auto &atomTypes = parent_->atomTypePopulations();
    isotopes_.erase([&atomTypes](auto value) { return !atomTypes.contains(value); });

    // Add in any used atom types that are not currently in the list
    for (const auto &[atomType, pop] : atomTypes)
    {
        if (!isotopes_.contains(atomType))
            isotopes_.set(atomType, Sears91::naturalIsotope(atomType->Z()));
    }
}

// Set Isotope associated to AtomType
void Isotopologue::setAtomTypeIsotope(const AtomType *atomType, Sears91::Isotope isotope)
{
    assert(atomType);

    isotopes_.set(atomType, isotope);
}

// Return Isotope for specified AtomType
Sears91::Isotope Isotopologue::atomTypeIsotope(const AtomType *atomType) const
{
    return isotopes_.contains(atomType) ? isotopes_.value(atomType) : Sears91::naturalIsotope(atomType->Z());
}

// Return AtomType/Isotope pairs list
KeyedVector<const AtomType *, Sears91::Isotope> &Isotopologue::isotopes() { return isotopes_; }
const KeyedVector<const AtomType *, Sears91::Isotope> &Isotopologue::isotopes() const { return isotopes_; }

/*
 * Serialisation
 */

// Express as a serialisable value
void Isotopologue::serialise(std::string tag, SerialisedValue &target) const
{
    auto &result = target[tag];
    for (auto &&[type, isotope] : isotopes_)
        result[type->name().data()] = Sears91::A(isotope);
}

void Isotopologue::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto &[name, value] : node.as_table())
    {
        if (value.is_string())
            continue;
        auto at = coreData.findAtomType(name);
        setAtomTypeIsotope(at.get(), Sears91::isotope(at->Z(), value.as_integer()));
    }
}
