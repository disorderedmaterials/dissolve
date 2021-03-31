// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/isotopologues.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include <algorithm>

Isotopologues::Isotopologues(const Species *species, int speciesPopulation)
    : species_(species), speciesPopulation_(speciesPopulation)
{
}

Isotopologues::~Isotopologues() {}

/*
 * Isotopologue Mix
 */

// Set associated Species
void Isotopologues::setSpecies(const Species *sp, int population)
{
    species_ = sp;
    speciesPopulation_ = population;
}

// Return associated Species
const Species *Isotopologues::species() const { return species_; }

// Return associated Species population
int Isotopologues::speciesPopulation() const { return speciesPopulation_; }

/// Prune defunct Isotopologue entries
void Isotopologues::pruneMissing()
{
    // Go through list of Isotopologues present in this mix, removing any that no longer exist
    mix_.erase(std::remove_if(mix_.begin(), mix_.end(),
                              [&](const auto &isoWeight) { return !species_->hasIsotopologue(isoWeight.isotopologue()); }),
               mix_.end());
}

// Add next available Isotopologue to list
bool Isotopologues::addNext()
{
    // NULL Pointer?
    if (species_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL Species pointer in Isotopologues::addNextIsotopologue().\n");
        return false;
    }

    // Check to see if the are any Isotopologues available to add
    if (mix_.size() == species_->nIsotopologues())
    {
        Messenger::warn("Can't add another Isotopologue to the mixture since there are none left for Species '{}'.\n",
                        species_->name());
        return false;
    }

    // Find unique (unused) Isotopologue
    Isotopologue *iso;
    for (iso = species_->isotopologues().first(); iso != nullptr; iso = iso->next())
        if (!contains(iso))
            break;

    if (iso == nullptr)
    {
        Messenger::error("Couldn't find an unused Isotopologue in Species '{}'.\n", species_->name());
        return false;
    }

    mix_.emplace_back(iso, 1.0);

    return true;
}

// Add specific Isotopologue to list
void Isotopologues::add(const Isotopologue *iso, double relativeWeight)
{
    // Search current list to see if the specified Isotopologue already exists
    if (contains(iso))
        Messenger::warn("Isotopologue '{}' (of Species '{}') already exists in Isotopologues, and it is being added again...\n",
                        iso->name(), species_->name());

    mix_.emplace_back(iso, relativeWeight);
}

// Set Isotopologue component in list
bool Isotopologues::set(const Isotopologue *iso, double relativeWeight)
{
    // NULL Pointer?
    if (iso == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL Isotopologue passed to Isotopologues::setIsotopologue().\n");
        return false;
    }

    // Find the specified Isotopologue
    auto it = std::find_if(mix_.begin(), mix_.end(), [iso](auto &isoWeight) { return isoWeight.isotopologue() == iso; });

    if (it == mix_.end())
    {
        Messenger::warn("Warning: Isotopologues does not contain the Isotopologue '{}', so its relative weight can't be set.\n",
                        iso->name());

        // TODO Raise exception
        return false;
    }
    else
        it->setWeight(relativeWeight);

    return true;
}

// Remove references to the specified Isotopologue
void Isotopologues::remove(const Isotopologue *iso)
{
    mix_.erase(
        std::remove_if(mix_.begin(), mix_.end(), [iso](const auto &isoWeight) { return isoWeight.isotopologue() == iso; }),
        mix_.end());
}

// Remove the specified IsotopologueWeight
void Isotopologues::remove(IsotopologueWeight *isoWeight)
{
    mix_.erase(std::remove_if(mix_.begin(), mix_.end(), [isoWeight](const auto &data) { return isoWeight == &data; }),
               mix_.end());
}

// Return whether the mix contains the specified Isotopologue
bool Isotopologues::contains(const Isotopologue *iso) const
{
    return std::any_of(mix_.cbegin(), mix_.cend(),
                       [iso](const IsotopologueWeight &isoWeight) { return isoWeight.isotopologue() == iso; });
}

// Return Isotopologue/weight mix
std::vector<IsotopologueWeight> &Isotopologues::mix() { return mix_; }

const std::vector<IsotopologueWeight> &Isotopologues::mix() const { return mix_; }

// Return number of Isotopologues in list
int Isotopologues::nIsotopologues() const { return mix_.size(); }

// Return total relative population
double Isotopologues::totalRelative() const
{
    double total = 0.0;

    for (auto isoWeight : mix_)
        total += isoWeight.weight();

    return total;
}

// Normalise total relative population to 1.0
void Isotopologues::normalise()
{
    double total = totalRelative();

    for (auto isoWeight : mix_)
        isoWeight.setWeight(isoWeight.weight() / total);
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool Isotopologues::deserialise(LineParser &parser, const CoreData &coreData)
{
    // Read Species name
    if (parser.getArgsDelim() != LineParser::Success)
        return false;
    species_ = coreData.findSpecies(parser.argsv(0));
    if (species_ == nullptr)
    {
        Messenger::error("Failed to find Species '{}' while reading Isotopologues.\n", parser.argsv(0));
        return false;
    }
    speciesPopulation_ = parser.argi(1);
    auto nIso = parser.argi(2);
    mix_.clear();
    for (auto n = 0; n < nIso; ++n)
    {
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Search for the named Isotopologue in the Species
        const Isotopologue *iso = species_->findIsotopologue(parser.argsv(0));
        if (!iso)
        {
            Messenger::error("Failed to find Isotopologue '{}' for Species '{}' while reading Isotopologues.\n",
                             parser.argsv(0), species_->name());
            return false;
        }

        add(iso, parser.argd(1));
    }

    return true;
}

// Write data through specified LineParser
bool Isotopologues::serialise(LineParser &parser) const
{
    // Write Species name, integer population, and number of isotopologues in the mix
    if (!parser.writeLineF("'{}'  {}  {}\n", species_->name(), speciesPopulation_, mix_.size()))
        return false;

    // Write Isotopologues
    for (auto isoWeight : mix_)
        if (!parser.writeLineF("{}  {}\n", isoWeight.isotopologue()->name(), isoWeight.weight()))
            return false;

    return true;
}
