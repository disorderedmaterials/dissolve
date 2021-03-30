// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/isotopologueset.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"

IsotopologueSetKeyword::IsotopologueSetKeyword(IsotopologueSet &set)
    : KeywordData<IsotopologueSet &>(KeywordBase::IsotopologueSetData, set)
{
}

IsotopologueSetKeyword::~IsotopologueSetKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IsotopologueSetKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
int IsotopologueSetKeyword::maxArguments() const { return 3; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool IsotopologueSetKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find specified Species (first argument)
    Species *sp = coreData.findSpecies(parser.argsv(startArg));
    if (!sp)
        return Messenger::error("Error defining Isotopologue reference - no Species named '{}' exists.\n",
                                parser.argsv(startArg + 1));

    // Finally, locate isotopologue definition for species (second argument)
    const Isotopologue *iso = sp->findIsotopologue(parser.argsv(startArg + 1));
    if (!iso)
        return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '{}' exists for Species '{}'.\n",
                                parser.argsv(startArg + 1), sp->name());

    // Add the isotopologue to the set
    data_.add(iso, parser.argd(startArg + 2));

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool IsotopologueSetKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto topes : data_.isotopologues())
        for (auto isoWeight : topes.mix())
        {
            if (!parser.writeLineF("{}{}  '{}'  '{}'  {}\n", prefix, keywordName, topes.species()->name(),
                                   isoWeight.isotopologue()->name(), isoWeight.weight()))
                return false;
        }

    return true;
}

/*
 * Data Management
 */

// Prune any references to the supplied Species in the contained data
void IsotopologueSetKeyword::removeReferencesTo(Species *sp) { data_.remove(sp); }

// Prune any references to the supplied Isotopologue in the contained data
void IsotopologueSetKeyword::removeReferencesTo(Isotopologue *iso) { data_.remove(iso); }
