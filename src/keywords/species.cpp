/*
    *** Keyword - Species
    *** src/keywords/species.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
bool SpeciesKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
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
bool SpeciesKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
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
