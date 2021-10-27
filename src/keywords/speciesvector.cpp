// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/speciesvector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesVectorKeyword::SpeciesVectorKeyword(std::vector<const Species *> &data)
    : KeywordBase(KeywordBase::SpeciesVectorData), data_(data)
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool SpeciesVectorKeyword::isDataEmpty() const { return data_.empty(); }

// Return reference to data vector
std::vector<const Species *> &SpeciesVectorKeyword::data() { return data_; }
const std::vector<const Species *> &SpeciesVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int SpeciesVectorKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Each argument is the name of a Species that we will add to our list
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

// Write keyword data to specified LineParser
bool SpeciesVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (isDataEmpty())
        return true;

    // Loop over list of Species
    std::string speciesString;
    for (const auto *sp : data_)
        speciesString += fmt::format("  '{}'", sp->name());

    if (!parser.writeLineF("{}{}  {}\n", prefix, keywordName, speciesString))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesVectorKeyword::removeReferencesTo(Species *sp)
{
    data_.erase(std::remove(data_.begin(), data_.end(), sp), data_.end());
}
