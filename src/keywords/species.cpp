// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/species.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesKeyword::SpeciesKeyword(Species *sp) : KeywordData<Species *>(KeywordBase::SpeciesData, sp) {}

SpeciesKeyword::~SpeciesKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int SpeciesKeyword::maxArguments() const { return 1; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Species (first argument)
    data_ = coreData.findSpecies(parser.argsv(startArg));
    if (!data_)
    {
        Messenger::error("Error setting Species - no Species named '{}' exists.\n", parser.argsv(startArg));
        return false;
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_)
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_->name()))
            return false;
    }
    else if (!parser.writeLineF("{}{}  '?_?'\n", prefix, name()))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesKeyword::removeReferencesTo(Species *sp)
{
    if (data_ == sp)
        data_ = nullptr;
}
