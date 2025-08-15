// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/isotopologues.h"
#include "base/lineParser.h"
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

// Return Isotopologue/weight mix
KeyedVector<const Isotopologue *, double> &Isotopologues::mix() { return mix_; }

const KeyedVector<const Isotopologue *, double> &Isotopologues::mix() const { return mix_; }

// Return summed weight over all isotopologues
double Isotopologues::summedWeight() const
{
    return std::accumulate(mix_.begin(), mix_.end(), 0.0, [](const auto acc, const auto &iso) { return acc + iso.second; });
}

// Return the normalised populations
KeyedVector<const Isotopologue *, double> Isotopologues::normalised() const
{
    auto sum = summedWeight();
    return mix_.operated([sum](const auto &value) { return value / sum; });
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

        mix_.set(iso, parser.argd(1));
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
    for (const auto &[iso, weight] : mix_)
        if (!parser.writeLineF("{}  {}\n", iso->name(), weight))
            return false;

    return true;
}

// Express as a serialisable value
SerialisedValue Isotopologues::serialise() const
{
    SerialisedValue result = {{"name", species_->name()}, {"population", speciesPopulation_}};

    SerialisedValue mix;
    for (const auto &[iso, weight] : mix_)
        mix[std::string(iso->name())] = weight;

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
                                throw toml::type_error(std::format("Cannot find iso {}", name), location);
                            mix_.set(iso, item.as_floating());
                        });
}
