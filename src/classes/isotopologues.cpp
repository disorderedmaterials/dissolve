// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/isotopologues.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "classes/species.h"
#include <numeric>

Isotopologues::Isotopologues(const Species *species, int speciesPopulation)
    : species_(species), speciesPopulation_(speciesPopulation)
{
}

/*
 * Isotopologue Mix
 */

// Set associated Species
void Isotopologues::setSpecies(const Species *sp, double population)
{
    species_ = sp;
    speciesPopulation_ = population;
}

// Return associated Species
const Species *Isotopologues::species() const { return species_; }

// Return associated Species population
double Isotopologues::speciesPopulation() const { return speciesPopulation_; }

// Add specific Isotopologue to list
void Isotopologues::add(const Isotopologue *iso, double relativeWeight)
{
    // Search current list to see if the specified Isotopologue already exists
    auto it = std::find_if(mix_.begin(), mix_.end(), [iso](auto &isoWeight) { return isoWeight.isotopologue() == iso; });
    if (it != mix_.end())
        it->addWeight(relativeWeight);
    else
        mix_.emplace_back(iso, relativeWeight);
}

// Set Isotopologue component in list
void Isotopologues::set(const Isotopologue *iso, double relativeWeight)
{
    assert(iso);

    // Find the specified Isotopologue
    auto it = std::find_if(mix_.begin(), mix_.end(), [iso](auto &isoWeight) { return isoWeight.isotopologue() == iso; });

    if (it == mix_.end())
        throw(std::runtime_error(
            fmt::format("Warning: Isotopologues does not contain the Isotopologue '{}', so its relative weight can't be set.\n",
                        iso->name())));

    it->setWeight(relativeWeight);
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

// Return summed weight over all isotopologues
double Isotopologues::summedWeight() const
{
    return std::accumulate(mix_.begin(), mix_.end(), 0.0, [](const auto acc, const auto &iso) { return acc + iso.weight(); });
}

// Normalise total relative population to 1.0
void Isotopologues::normalise()
{
    auto sum = summedWeight();

    for (auto &isoWeight : mix_)
        isoWeight.setWeight(isoWeight.weight() / sum);
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
    for (const auto &isoWeight : mix_)
        if (!parser.writeLineF("{}  {}\n", isoWeight.isotopologue()->name(), isoWeight.weight()))
            return false;

    return true;
}

// Express as a serialisable value
SerialisedValue Isotopologues::serialise() const
{
    SerialisedValue result = {{"name", species_->name()}, {"population", speciesPopulation_}};

    SerialisedValue mix;
    for (const auto &isoWeight : mix_)
        mix[std::string(isoWeight.isotopologue()->name())] = isoWeight.weight();

    result["mix"] = mix;

    return result;
}

// Read values from a serialisable value
void Isotopologues::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    species_ = coreData.findSpecies(toml::find<std::string>(node, "name"));
    speciesPopulation_ = toml::find<double>(node, "population");

    auto location = node.location();
    Serialisable::toMap(node, "mix",
                        [&coreData, &location, this](const auto &name, const auto &item)
                        {
                            auto iso = species_->findIsotopologue(name);
                            if (!iso)
                                throw toml::type_error(fmt::format("Cannot find iso {}", name), location);
                            add(iso, item.as_floating());
                        });
}
