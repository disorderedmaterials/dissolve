/*
    *** Keyword - Species Site
    *** src/keywords/speciessite.cpp
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
bool SpeciesSiteKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Find target Species (first argument)
    Species *sp = coreData.findSpecies(parser.argsv(startArg));
    if (!sp)
    {
        Messenger::error("Error setting SpeciesSite - no Species named '{}' exists.\n", parser.argsv(startArg));
        return false;
    }

    // Find specified Site (second argument) in the Species
    data_ = sp->findSite(parser.argsv(startArg + 1));
    if (!data_)
        return Messenger::error("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n",
                                parser.argsv(startArg + 1), sp->name());
    if (axesRequired_ && (!data_->hasAxes()))
        return Messenger::error(
            "Can't select site '{}' for keyword '{}', as the keyword requires axes specifications to be present.\n",
            data_->name(), name());

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool SpeciesSiteKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
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
