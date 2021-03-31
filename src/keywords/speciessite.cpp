// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/speciessite.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesSiteKeyword::SpeciesSiteKeyword(SpeciesSite *site, bool axesRequired)
    : KeywordData<SpeciesSite *>(KeywordBase::SpeciesSiteData, site)
{
    axesRequired_ = axesRequired;
}

SpeciesSiteKeyword::~SpeciesSiteKeyword() {}

/*
 * Specification
 */

// Return whether axes are required for the site
bool SpeciesSiteKeyword::axesRequired() const { return axesRequired_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesSiteKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int SpeciesSiteKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesSiteKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Species (first argument)
    Species *sp = coreData.findSpecies(parser.argsv(startArg));
    if (!sp)
    {
        Messenger::error("Error setting SpeciesSite - no Species named '{}' exists.\n", parser.argsv(startArg));
        return false;
    }

    // Find specified Site (second argument) in the Species
    auto site = sp->findSite(parser.argsv(startArg + 1));
    if (!site)
        return Messenger::error("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n",
                                parser.argsv(startArg + 1), sp->name());
    data_ = &site->get();
    if (axesRequired_ && (!data_->hasAxes()))
        return Messenger::error(
            "Can't select site '{}' for keyword '{}', as the keyword requires axes specifications to be present.\n",
            data_->name(), name());

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesSiteKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_)
    {
        if (!parser.writeLineF("{}{}  '{}'  '{}'\n", prefix, keywordName, data_->parent()->name(), data_->name()))
            return false;
    }
    else if (!parser.writeLineF("{}{}  '?_?'  '?_?'\n", prefix, name()))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesSiteKeyword::removeReferencesTo(Species *sp)
{
    if (data_ && (data_->parent() == sp))
        data_ = nullptr;
}

// Prune any references to the supplied Site in the contained data
void SpeciesSiteKeyword::removeReferencesTo(SpeciesSite *spSite)
{
    if (data_ == spSite)
        data_ = nullptr;
}
