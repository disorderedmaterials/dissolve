/*
	*** Isotopologue Collection
	*** src/classes/isotopologuecollection.cpp
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

// Remove any sets from the collection that are empty
void IsotopologueCollection::pruneEmptySets()
{
	IsotopologueSet* set = isotopologueSets_.first();
	while (set != NULL)
	{
		IsotopologueSet* nextSet = set->next();
		if (set->nIsotopologues() == 0) isotopologueSets_.remove(set);

		set = nextSet;
	}
}

// Clear all existing data
void IsotopologueCollection::clear()
{
	isotopologueSets_.clear();
}

// Add Isotopologue weight for the specified Configuration / Species
void IsotopologueCollection::add(Configuration* cfg, Isotopologue* iso, double relativeWeight)
{
	// Check if the set already exists
	IsotopologueSet* set = NULL;
	for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
	if (!set)
	{
		set = isotopologueSets_.add();
		set->setParentCollection(this);
		set->setConfiguration(cfg);
	}

	set->add(iso, relativeWeight);
}

// Remove the specified set from the collection
void IsotopologueCollection::remove(IsotopologueSet* set)
{
	isotopologueSets_.remove(set);
}

// Remove the Configuration from the collection
void IsotopologueCollection::remove(Configuration* cfg)
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg)
	{
		isotopologueSets_.remove(set);
		break;
	}
}

// Remove the Species from the specified set
void IsotopologueCollection::remove(IsotopologueSet* set, Species* sp)
{
	set->remove(sp);

	pruneEmptySets();
}

// Remove the IsotopologueWeight from the specified set
void IsotopologueCollection::remove(IsotopologueSet* set, IsotopologueWeight* isoWeight)
{
	set->remove(isoWeight);

	pruneEmptySets();
}

// Remove any occurrences of the specified Species from the collection
void IsotopologueCollection::remove(Species* sp)
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) set->remove(sp);

	pruneEmptySets();
}

// Remove any occurrences of the specified Isotopologue from the collection
void IsotopologueCollection::remove(Isotopologue* iso)
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) set->remove(iso);

	pruneEmptySets();
}

// Return defined sets
List<IsotopologueSet>& IsotopologueCollection::isotopologueSets()
{
	return isotopologueSets_;
}

// Return whether a set exists for the supplied Configuration
bool IsotopologueCollection::contains(const Configuration* cfg) const
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) return true;

	return false;
}

// Return IsotopologueSet for the specified Configuration
const IsotopologueSet* IsotopologueCollection::isotopologueSet(const Configuration* cfg) const
{
	for (IsotopologueSet* set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) return set;

	return NULL;
}

// Return whether the Species has a defined set of isotopologues in the specified Configuration
bool IsotopologueCollection::contains(const Configuration* cfg, const Species* sp) const
{
	IsotopologueSet* set = NULL;
	for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
	if (!set) return false;

	return set->contains(sp);
}

// Return Isotopologues for the Species in the specified Configuration
const Isotopologues* IsotopologueCollection::isotopologues(const Configuration* cfg, const Species* sp) const
{
	IsotopologueSet* set = NULL;
	for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
	if (!set) return NULL;

	return set->isotopologues(sp);
}

// Complete the collection by making sure it contains every Species in every Configuration in the supplied list
void IsotopologueCollection::complete(const RefList<Configuration>& configurations)
{
	for (Configuration* cfg : configurations)
	{
		// Retrieve / create a set for this Configuration
		IsotopologueSet* set = NULL;
		for (set = isotopologueSets_.first(); set != NULL; set = set->next()) if (set->configuration() == cfg) break;
		if (!set)
		{
			set = isotopologueSets_.add();
			set->setParentCollection(this);
			set->setConfiguration(cfg);
		}

		// Loop over Species in the Configuration
		ListIterator<SpeciesInfo> spInfoIterator(cfg->usedSpecies());
		while (SpeciesInfo* spInfo = spInfoIterator.iterate())
		{
			// If the Species already exists in our set, nothing more to do...
			if (set->contains(spInfo->species())) continue;

			// Add the natural isotopologue for this Species
			add(cfg, spInfo->species()->naturalIsotopologue(), 1.0);
		}
	}
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
		set->setParentCollection(this);
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
