/*
    *** Keyword - Species RefList
    *** src/keywords/speciesreflist.cpp
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

#include "keywords/speciesreflist.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesRefListKeyword::SpeciesRefListKeyword(RefList<Species> &references)
    : KeywordData<RefList<Species> &>(KeywordBase::SpeciesRefListData, references)
{
}

SpeciesRefListKeyword::~SpeciesRefListKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool SpeciesRefListKeyword::isDataEmpty() const { return data_.nItems() > 0; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesRefListKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int SpeciesRefListKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesRefListKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Each argument is the name of a Species that we will add to our list
    for (int n = startArg; n < parser.nArgs(); ++n)
    {
        Species *sp = coreData.findSpecies(parser.argsv(n));
        if (!sp)
            return Messenger::error("Error defining Species targets - no Species named '{}' exists.\n", parser.argsv(n));

        data_.append(sp);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of Species
    std::string speciesString;
    for (Species *sp : data_)
        speciesString += fmt::format("  {}", sp->name());

    if (!parser.writeLineF("{}{}  {}\n", prefix, keywordName, speciesString))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesRefListKeyword::removeReferencesTo(Species *sp) { data_.remove(sp); }
