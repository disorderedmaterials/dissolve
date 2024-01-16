// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/speciesVector.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
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

// Express as a serialisable value
SerialisedValue SpeciesVectorKeyword::serialise() const
{
    return fromVector(data_, [](const auto *item) { return item->name(); });
}

// Read values from a serialisable value
void SpeciesVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [&coreData, this](const auto &item)
             {
                 auto title = toml::get<std::string>(item);
                 auto *species = coreData.findSpecies(title);
                 if (!species)
                     throw toml::type_error(fmt::format("No Species named '{}' exists.\n", title), item.location());

                 data_.push_back(species);
             });
}

// Has not changed from initial value
bool SpeciesVectorKeyword::isDefault() const { return data_.empty(); }
