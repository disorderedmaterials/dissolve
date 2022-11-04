// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/speciesvector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "templates/algorithms.h"

SpeciesVectorKeyword::SpeciesVectorKeyword(std::vector<const Species *> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data vector
std::vector<const Species *> &SpeciesVectorKeyword::data() { return data_; }
const std::vector<const Species *> &SpeciesVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> SpeciesVectorKeyword::maxArguments() const { return 99; }

// Deserialise from supplied LineParser, starting at given argument offset
bool SpeciesVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Each argument is the name of a Species
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        const auto *sp = coreData.findSpecies(parser.argsv(n));
        if (!sp)
            return Messenger::error("Error reading keyword '{}' - no Species named '{}' exists.\n", name(), parser.argsv(n));

        data_.push_back(sp);
    }

    return true;
}

// Serialise data to specified LineParser
bool SpeciesVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName,
                             joinStrings(data_, "  ", [](const auto *sp) { return sp->name(); }));
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesVectorKeyword::removeReferencesTo(Species *sp)
{
    data_.erase(std::remove(data_.begin(), data_.end(), sp), data_.end());
}

// Express as a tree node
SerialisedValue SpeciesVectorKeyword::serialise() const
{
    auto result = toml::array{};
    std::transform(data_.begin(), data_.end(), std::back_inserter(result), [](const auto *item) { return item->name(); });
    return result;
}

// Read values from a tree node
void SpeciesVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto item : node.as_array())
    {
        auto title = toml::get<std::string>(item);
        auto *species = coreData.findSpecies(title);
        if (!species)
            throw toml::err(fmt::format("No Species named '{}' exists.\n", title));

        data_.push_back(species);
    }
}

// Has not changed from inital value
bool SpeciesVectorKeyword::isDefault() const { return data_.empty(); }
