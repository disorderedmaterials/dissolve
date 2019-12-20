/*
	*** Isotopologue Set
	*** src/classes/isotopologueset.cpp
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

#include "classes/isotopologueset.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"

// Constructor
IsotopologueSet::IsotopologueSet() : ListItem<IsotopologueSet>()
{
	clear();
}

// Destructor
IsotopologueSet::~IsotopologueSet()
{
}

/*
 * Mix Definitions
 */

// Clear all existing data
void IsotopologueSet::clear()
{
	configuration_ = NULL;
	isotopologues_.clear();
}

// Set Configuration in which the Species are used
void IsotopologueSet::setConfiguration(Configuration* cfg)
{
	configuration_ = cfg;
}

// Return Configuration in which the Species are used
Configuration* IsotopologueSet::configuration() const
{
	return configuration_;
}

// Add Isotopologue with the specified relative weight
void IsotopologueSet::add(Isotopologue* iso, double relativeWeight)
{
	// Find the parent Species of the isotopologue in our list
	Isotopologues* topes = NULL;
	for (topes = isotopologues_.first(); topes != NULL; topes = topes->next()) if (topes->species() == iso->parent()) break;

	// Create a new Isotopologues if we need it
	if (!topes)
	{
		topes = isotopologues_.add();
		topes->setSpecies(iso->parent(), 0);
	}

	topes->add(iso, relativeWeight);
}

// Remove specified Species from the list (if it exists)
void IsotopologueSet::remove(Species* sp)
{
	Isotopologues* topes = isotopologues(sp);
	if (topes) isotopologues_.remove(topes);
}

// Remove any occurrences of the specified Isotopologue
void IsotopologueSet::remove(Isotopologue* iso)
{
	Isotopologues* topes = isotopologues(iso->parent());
	if (topes)
	{
		topes->remove(iso);

		// Check for an empty list...
		if (topes->nIsotopologues() == 0) isotopologues_.remove(topes);
	}
}

// Return whether an IsotopologueSet for the specified Species exists
bool IsotopologueSet::contains(const Species* sp) const
{
	for (Isotopologues* topes = isotopologues_.first(); topes != NULL; topes = topes->next()) if (topes->species() == sp) return true;

	return false;
}

// Return IsotopologueSet for the specified Species
Isotopologues* IsotopologueSet::isotopologues(const Species* sp)
{
	for (Isotopologues* topes = isotopologues_.first(); topes != NULL; topes = topes->next()) if (topes->species() == sp) return topes;

	return  NULL;
}

// Return number of Isotopologues defined
int IsotopologueSet::nIsotopologues() const
{
	return isotopologues_.nItems();
}

// Return list of all Isotopologues
const List<Isotopologues>& IsotopologueSet::isotopologues() const
{
	return isotopologues_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* IsotopologueSet::itemClassName()
{
	return "IsotopologueSet";
}

// Read data through specified LineParser
bool IsotopologueSet::read(LineParser& parser, const CoreData& coreData)
{
	clear();

	// Find target Configuration (first argument) and number of Species defined (second argument)
	configuration_ = coreData.findConfiguration(parser.argc(0));
	if (!configuration_)
	{
		Messenger::error("Error reading IsotopologueSet - no Configuration named '%s' exists.\n", parser.argc(0));
		return false;
	}
	const int nSpecies = parser.argi(1);

	for (int n=0; n<nSpecies; ++n)
	{
		// Add a new isotopologue set and read it
		Isotopologues* topes = isotopologues_.add();
		if (!topes->read(parser, coreData)) return false;
	}

	return true;
}

// Write data through specified LineParser
bool IsotopologueSet::write(LineParser& parser)
{
	// Write Configuration name and number of Isotopologues we have defined
	if (!parser.writeLineF("'%s'  %i\n", configuration_->name(), isotopologues_.nItems())) return false;

	// Write details for each set of Isotopologues
	ListIterator<Isotopologues> isotopologuesIterator(isotopologues_);
	while (Isotopologues* topes = isotopologuesIterator.iterate()) if (!topes->write(parser)) return false;

	return true;
}
