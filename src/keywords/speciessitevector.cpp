// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/speciessitevector.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesSiteVectorKeyword::SpeciesSiteVectorKeyword(std::vector<const SpeciesSite *> sites, bool axesRequired)
    : KeywordData<std::vector<const SpeciesSite *>>(KeywordData::SpeciesSiteVectorData, std::move(sites))
{
    axesRequired_ = axesRequired;
}

SpeciesSiteVectorKeyword::~SpeciesSiteVectorKeyword() = default;

/*
 * Specification
 */

// Return whether axes are required for the site
bool SpeciesSiteVectorKeyword::axesRequired() const { return axesRequired_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesSiteVectorKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int SpeciesSiteVectorKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesSiteVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments
    for (int n = startArg; n < parser.nArgs() - 1; n += 2)
    {
        // Find target Species (first argument)
        Species *sp = coreData.findSpecies(parser.argsv(n));
        if (!sp)
            return Messenger::error("Error adding SpeciesSite - no Species named '{}' exists.\n", parser.argsv(n));

        // Find specified Site (second argument) in the Species
        auto speciesSite = sp->findSite(parser.argsv(n + 1));
        if (!speciesSite)
            return Messenger::error("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n",
                                    parser.argsv(n + 1), sp->name());
        if (axesRequired_ && (!speciesSite->hasAxes()))
            return Messenger::error("Can't add site '{}' to keyword '{}', as the keyword requires axes "
                                    "specifications for all sites.\n",
                                    speciesSite->name(), name());

        // Add site to the list
        data_.push_back(speciesSite);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesSiteVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // If there are no sites in the list, no need to write anything
    if (data_.empty())
        return true;

    // Loop over list of SpeciesSiteReferences
    std::string sites;
    for (auto *site : data_)
        sites += fmt::format("  '{}'  '{}'", site->parent()->name(), site->name());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, sites))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesSiteVectorKeyword::removeReferencesTo(Species *sp)
{
    data_.erase(std::remove_if(data_.begin(), data_.end(), [sp](const auto *site) { return site->parent() == sp; }),
                data_.end());
}

// Prune any references to the supplied SpeciesSite in the contained data
void SpeciesSiteVectorKeyword::removeReferencesTo(SpeciesSite *spSite)
{
    data_.erase(std::remove(data_.begin(), data_.end(), spSite), data_.end());
}
