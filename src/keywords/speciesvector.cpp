// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/speciesvector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "templates/algorithms.h"

SpeciesVectorKeyword::SpeciesVectorKeyword(std::vector<const Species *> &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool SpeciesVectorKeyword::isDataEmpty() const { return data_.empty(); }

// Return reference to data vector
dissolve::any_ptr SpeciesVectorKeyword::data() const { return data_; }

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

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool SpeciesVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (isDataEmpty())
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
