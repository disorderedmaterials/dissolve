/*
	*** Isotopologue Collection
	*** src/classes/isotopologuecollection.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/isotopologuecollection.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"

// Constructor
IsotopologueCollection::IsotopologueCollection()
{
	clear();
}

// Destructor
IsotopologueCollection::~IsotopologueCollection()
{
}

/*
 * Sets
 */

// Clear all existing data
void IsotopologueCollection::clear()
{
	isotopologueSets_.clear();
}

// Add IsotopologueSet for the specified Configuration
IsotopologueSet* IsotopologueCollection::add(Configuration* cfg)
{
	// Check if the set already exists
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next())
	{
		if (set->configuration() == cfg)
		{
			Messenger::warn("Collection already exists for the configuration '%s', so returning that...\n", cfg->name());
			return set;
		}
	}

	IsotopologueSet* set = isotopologueSets_.add();
	set->setConfiguration(cfg);

	return set;
}

// Return defined sets
List<IsotopologueSet>& IsotopologueCollection::isotopologueSets()
{
	return isotopologueSets_;
}

// Return whether a set exists for the supplied Configuration
bool IsotopologueCollection::hasIsotopologueSet(const Configuration* cfg) const
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) return true;

	return false;
}

// Return IsotopologueSet for the specified Configuration
IsotopologueSet* IsotopologueCollection::isotopologueSet(const Configuration* cfg)
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) return set;

	return NULL;
}

// Return whether the Species has a defined set of isotopologues in the specified Configuration
bool IsotopologueCollection::hasIsotopologues(const Configuration* cfg, const Species* sp) const
{
	IsotopologueSet* set = NULL;
	for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
	if (!set) return false;

	return set->contains(sp);
}

// Return Isotopologues for the Species in the specified Configuration
Isotopologues* IsotopologueCollection::isotopologues(const Configuration* cfg, const Species* sp)
{
	IsotopologueSet* set = NULL;
	for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
	if (!set) return NULL;

	return set->isotopologues(sp);
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* IsotopologueCollection::itemClassName()
{
	return "IsotopologueCollection";
}

// Read data through specified LineParser
bool IsotopologueCollection::read(LineParser& parser, const CoreData& coreData)
{
	clear();

	// Read in number of Configurations in the collection
	const int nConfigurations = parser.argi(0);

	for (int n=0; n<nConfigurations; ++n)
	{
		// Add a new isotopologue set and read it
		IsotopologueSet* set = isotopologueSets_.add();
		if (!set->read(parser, coreData)) return false;
	}

	return true;
}

// Write data through specified LineParser
bool IsotopologueCollection::write(LineParser& parser)
{
	// Write number of Configurations in the collection
	if (!parser.writeLineF("%i\n", isotopologueSets_.nItems())) return false;

	// Write details for each set of Isotopologues
	ListIterator<IsotopologueSet> setIterator(isotopologueSets_);
	while (IsotopologueSet* set = setIterator.iterate()) if (!set->write(parser)) return false;

	return true;
}
