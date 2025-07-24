// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/atomTypeSet.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include <numeric>
#include <ranges>

AtomTypeSet::AtomTypeSet(const AtomTypeSet &source) { (*this) = source; }

AtomTypeSet &AtomTypeSet::operator=(const AtomTypeSet &source)
{
    set_ = source.set_;
    return *this;
}

/*
 * Set
 */

// Clear all data
void AtomTypeSet::clear() { set_.clear(); }

// Zero populations of all types in the list
void AtomTypeSet::zero()
{
    for (auto &data : std::views::values(set_))
        data = 0.0;
}

// Add (to) the named type
void AtomTypeSet::add(std::string_view atomTypeName, double population) { set_[std::string(atomTypeName)] += population; }

// Return number of AtomType/Isotopes
int AtomTypeSet::nItems() const { return set_.size(); }

// Return starting iterator
std::map<std::string, double>::const_iterator AtomTypeSet::begin() const { return set_.begin(); }

// Return ending iterator
std::map<std::string, double>::const_iterator AtomTypeSet::end() const { return set_.end(); }

// Return total population of all types
double AtomTypeSet::totalPopulation() const
{
    auto total = 0.0;
    for (auto &data : std::views::values(set_))
        total += data;
    return total;
}

// Return map of fractional populations of types
std::map<std::string, double> AtomTypeSet::fractional() const
{
    auto result = set_;
    auto totalPop = totalPopulation();
    for (auto &data : std::views::values(result))
        data /= totalPop;
    return result;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool AtomTypeSet::deserialise(LineParser &parser, const CoreData &coreData)
{
    set_.clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nItems = parser.argi(0);
    for (auto n = 0; n < nItems; ++n)
    {
        // Line Contains: AtomType name, population
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto atomType = coreData.findAtomType(parser.argsv(0));
        if (!atomType)
            return Messenger::error("Could not find atom type {}.", parser.argsv(0));
        set_[parser.args(0)] = parser.argd(1);
    }

    return true;
}

// Write data through specified LineParser
bool AtomTypeSet::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{}  # nItems\n", set_.size()))
        return false;

    for (auto &[name, data] : set_)
    {
        // Line Contains: AtomType name, population
        if (!parser.writeLineF("{} {}\n", name, data))
            return false;
    }

    return true;
}
