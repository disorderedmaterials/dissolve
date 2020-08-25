/*
    *** Keyword - SpeciesSite RefList
    *** src/keywords/speciessitereflist.cpp
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

#include "keywords/speciessitereflist.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"

SpeciesSiteRefListKeyword::SpeciesSiteRefListKeyword(RefList<SpeciesSite> &references, bool axesRequired)
    : KeywordData<RefList<SpeciesSite> &>(KeywordData::SpeciesSiteRefListData, references)
{
    axesRequired_ = axesRequired;
}

SpeciesSiteRefListKeyword::~SpeciesSiteRefListKeyword() {}

/*
 * Specification
 */

// Return whether axes are required for the site
bool SpeciesSiteRefListKeyword::axesRequired() const { return axesRequired_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesSiteRefListKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int SpeciesSiteRefListKeyword::maxArguments() const { return 99; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool SpeciesSiteRefListKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Loop over arguments
    for (int n = startArg; n < parser.nArgs() - 1; n += 2)
    {
        // Find target Species (first argument)
        Species *sp = coreData.findSpecies(parser.argsv(n));
        if (!sp)
        {
            Messenger::error("Error adding SpeciesSite - no Species named '{}' exists.\n", parser.argsv(startArg));
            return false;
        }

        // Find specified Site (second argument) in the Species
        SpeciesSite *site = sp->findSite(parser.argsv(n + 1));
        if (!site)
            return Messenger::error("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n",
                                    parser.argsv(n + 1), sp->name());
        if (axesRequired_ && (!site->hasAxes()))
            return Messenger::error("Can't add site '{}' to keyword '{}', as the keyword requires axes "
                                    "specifications for all sites.\n",
                                    site->name(), name());

        // Add site to the list
        data_.append(site);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesSiteRefListKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // If there are no sites in the list, no need to write anything
    if (data_.nItems() == 0)
        return true;

    // Loop over list of SpeciesSiteReferences
    std::string sites;
    for (SpeciesSite *site : data_)
        sites += fmt::format("  '{}'  '{}'", site->parent()->name(), site->name());

    if (!parser.writeLineF("{}{}{}\n", prefix, keywordName, sites))
        return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesSiteRefListKeyword::removeReferencesTo(Species *sp)
{
    RefListItem<SpeciesSite> *ri = data_.first(), *nextItem;
    while (ri)
    {
        nextItem = ri->next();
        if (ri->item()->parent() == sp)
            data_.remove(ri);
        ri = nextItem;
    }
}

// Prune any references to the supplied SpeciesSite in the contained data
void SpeciesSiteRefListKeyword::removeReferencesTo(SpeciesSite *spSite) { data_.remove(spSite); }
