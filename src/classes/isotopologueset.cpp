/*
    *** Isotopologue Set
    *** src/classes/isotopologueset.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/isotopologueset.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include <algorithm>

IsotopologueSet::IsotopologueSet(IsotopologueCollection *parent, Configuration *cfg)
    : parentCollection_(parent), configuration_(cfg)
{
}

IsotopologueSet::~IsotopologueSet() {}

/*
 * Parent Collection
 */

// Set parent IsotopologueCollection in which this set exists
void IsotopologueSet::setParentCollection(IsotopologueCollection *parent) { parentCollection_ = parent; }

// Parent IsotopologueCollection in which this set exists
IsotopologueCollection *IsotopologueSet::parentCollection() const { return parentCollection_; }

/*
 * Mix Definitions
 */

// Clear all existing data
void IsotopologueSet::clear()
{
    configuration_ = nullptr;
    isotopologues_.clear();
}

// Set Configuration in which the Species are used
void IsotopologueSet::setConfiguration(Configuration *cfg) { configuration_ = cfg; }

// Return Configuration in which the Species are used
Configuration *IsotopologueSet::configuration() const { return configuration_; }

// Add Isotopologue with the specified relative weight
void IsotopologueSet::add(Isotopologue *iso, double relativeWeight)
{
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [iso](auto &data) { return data.species() == iso->parent(); });
    if (it != isotopologues_.end())
        it->add(iso, relativeWeight);
    else
    {
        isotopologues_.emplace_back(iso->parent(), 0);
        isotopologues_.back().add(iso, relativeWeight);
    }
}

// Remove specified Species from the list (if it exists)
void IsotopologueSet::remove(Species *sp)
{
    isotopologues_.erase(
        std::remove_if(isotopologues_.begin(), isotopologues_.end(), [sp](const auto &data) { return data.species() == sp; }),
        isotopologues_.end());
}

// Remove any occurrences of the specified Isotopologue
void IsotopologueSet::remove(Isotopologue *iso)
{
    // Get parent Isotopologues from the contained Species pointer
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [iso](auto &data) { return data.species() == iso->parent(); });

    if (it != isotopologues_.end())
    {
        it->remove(iso);

        // Check for Isotopologues now being empty
        if (it->nIsotopologues() == 0)
            isotopologues_.erase(it);
    }
}

// Remove the specified IsotopologueWeight
void IsotopologueSet::remove(IsotopologueWeight *isoWeight)
{
    // Get Isotopologues related to the IsotopologueWeight's Species pointer
    auto it = std::find_if(isotopologues_.begin(), isotopologues_.end(),
                           [isoWeight](auto &data) { return data.species() == isoWeight->isotopologue()->parent(); });

    if (it != isotopologues_.end())
    {
        it->remove(isoWeight);

        // Check for Isotopologues now being empty
        if (it->nIsotopologues() == 0)
            isotopologues_.erase(it);
    }
}

// Return whether Isotopologues for the specified Species exists
bool IsotopologueSet::contains(const Species *sp) const
{
    return std::any_of(isotopologues_.cbegin(), isotopologues_.cend(),
                       [sp](const Isotopologues &mix) { return mix.species() == sp; });
}

// Return IsotopologueSet for the specified Species
OptionalReferenceWrapper<const Isotopologues> IsotopologueSet::getIsotopologues(const Species *sp) const
{
    auto it =
        std::find_if(isotopologues_.cbegin(), isotopologues_.cend(), [sp](const auto &data) { return data.species() == sp; });
    if (it == isotopologues_.end())
        return {};

    return *it;
}

// Return number of Isotopologues defined
int IsotopologueSet::nIsotopologues() const { return isotopologues_.size(); }

// Return vector of all Isotopologues
std::vector<Isotopologues> &IsotopologueSet::isotopologues() { return isotopologues_; }

// Return vector of all Isotopologues (const)
const std::vector<Isotopologues> &IsotopologueSet::constIsotopologues() const { return isotopologues_; }

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view IsotopologueSet::itemClassName() { return "IsotopologueSet"; }

// Read data through specified LineParser
bool IsotopologueSet::read(LineParser &parser, CoreData &coreData)
{
    clear();

    // Find target Configuration (first argument) and number of Species defined (second argument)
    configuration_ = coreData.findConfiguration(parser.argsv(0));
    if (!configuration_)
    {
        Messenger::error("Error reading IsotopologueSet - no Configuration named '{}' exists.\n", parser.argsv(0));
        return false;
    }
    const auto nSpecies = parser.argi(1);

    for (int n = 0; n < nSpecies; ++n)
    {
        // Add a new isotopologue set and read it
        isotopologues_.emplace_back();
        if (!isotopologues_.back().read(parser, coreData))
            return false;
    }

    return true;
}

// Write data through specified LineParser
bool IsotopologueSet::write(LineParser &parser)
{
    // Write Configuration name and number of Isotopologues we have defined
    if (!parser.writeLineF("'{}'  {}\n", configuration_->name(), isotopologues_.size()))
        return false;

    // Write details for each set of Isotopologues
    for (auto topes : isotopologues_)
        if (!topes.write(parser))
            return false;

    return true;
}
