/*
    *** Keyword - Isotopologue Collection
    *** src/keywords/isotopologuecollection.cpp
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

#include "keywords/isotopologuecollection.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"

IsotopologueCollectionKeyword::IsotopologueCollectionKeyword(IsotopologueCollection &collection,
                                                             const RefList<Configuration> &allowedConfigurations)
    : KeywordData<IsotopologueCollection &>(KeywordBase::IsotopologueCollectionData, collection),
      allowedConfigurations_(allowedConfigurations)
{
}

IsotopologueCollectionKeyword::~IsotopologueCollectionKeyword() {}

/*
 * Associated Configurations
 */

// Return associated Configurations, to which the IsotopologueCollection may refer
const RefList<Configuration> &IsotopologueCollectionKeyword::allowedConfigurations() const { return allowedConfigurations_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IsotopologueCollectionKeyword::minArguments() const { return 4; }

// Return maximum number of arguments accepted
int IsotopologueCollectionKeyword::maxArguments() const { return 4; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool IsotopologueCollectionKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    // Find target Configuration (first argument)
    Configuration *cfg = coreData.findConfiguration(parser.argsv(startArg));
    if (!cfg)
    {
        Messenger::error("Error defining Isotopologue reference - no Configuration named '{}' exists.\n",
                         parser.argsv(startArg));
        return false;
    }

    // Is this Configuration allowed?
    if (!allowedConfigurations_.contains(cfg))
        return Messenger::error("Configuration '{}' is not a valid target for this isotopologue collection.\n", cfg->name());

    // Find specified Species (second argument)
    Species *sp = coreData.findSpecies(parser.argsv(startArg + 1));
    if (!sp)
        return Messenger::error("Error defining Isotopologue reference - no Species named '{}' exists.\n",
                                parser.argsv(startArg + 1));

    // Finally, locate isotopologue definition for species (third argument)
    Isotopologue *iso = sp->findIsotopologue(parser.argsv(startArg + 2));
    if (!iso)
        return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '{}' exists for Species '{}'.\n",
                                parser.argsv(startArg + 2), sp->name());

    // Add the isotopologue to the collection
    data_.add(cfg, iso, parser.argd(startArg + 3));

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool IsotopologueCollectionKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of IsotopologueReferences
    for (auto set : data_.isotopologueSets())
    {
        for (auto topes : set.isotopologues())
        {
            for (auto isoWeight : topes.mix())
            {
                if (!parser.writeLineF("{}{}  '{}'  '{}'  '{}'  {}\n", prefix, keywordName, set.configuration()->name(),
                                       topes.species()->name(), isoWeight.isotopologue()->name(), isoWeight.weight()))
                    return false;
            }
        }
    }

    return true;
}

/*
 * Data Management
 */

// Prune any references to the supplied Species in the contained data
void IsotopologueCollectionKeyword::removeReferencesTo(Species *sp) { data_.remove(sp); }

// Prune any references to the supplied Isotopologue in the contained data
void IsotopologueCollectionKeyword::removeReferencesTo(Isotopologue *iso) { data_.remove(iso); }
