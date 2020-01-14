/*
	*** Species Definition - Isotopologues
	*** src/classes/species_isotopologues.cpp
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

#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Update current Isotopologues
void Species::updateIsotopologues()
{
	for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next()) iso->update();
}

// Update and return natural isotopologue
Isotopologue* Species::naturalIsotopologue()
{
	if (naturalIsotopologuePoint_ != atomTypesVersion_)
	{
		naturalIsotopologue_.update();

		naturalIsotopologuePoint_ = atomTypesVersion_;
	}

	return &naturalIsotopologue_;
}

// Add a new Isotopologue to this species
Isotopologue* Species::addIsotopologue(const char* baseName)
{
	Isotopologue* iso = isotopologues_.add();
	iso->setParent(this);
	iso->setName(uniqueIsotopologueName(baseName));
	iso->update();

	return iso;
}

// Remove specified Isotopologue from this Species
void Species::removeIsotopologue(Isotopologue* iso)
{
	if (iso == NULL) Messenger::error("NULL_POINTER - NULL Isotopologue passed to Species::removeIsotopologue().\n");
	else if (isotopologues_.contains(iso))
	{
		CharString tempName = iso->name();
		isotopologues_.remove(iso);
		Messenger::print("Removed Isotopologue '%s' from Species '%s'.\n", tempName.get(), name_.get());
	}
	else
	{
		Messenger::print("BAD_REMOVE - Can't remove specified Isotopologue '%s' from Species '%s' since it doesn't exist.\n", iso->name(), name_.get());
		if (iso->parent() == NULL) Messenger::print("BAD_CLASS - No parent pointer set in Isotopologue '%s'.\n", iso->name());
		else Messenger::print("BAD_REMOVE - Parent Species (%s) of Isotopologue '%s' is different from this one (%s).\n", iso->parent()->name(), iso->name(), name());
	}
}

// Return number of defined Isotopologues
int Species::nIsotopologues() const
{
	return isotopologues_.nItems();
}

// Return nth Isotopologue in the list
Isotopologue* Species::isotopologue(int n)
{
	return isotopologues_[n];
}

// Return Isotopologue List
const List<Isotopologue>& Species::isotopologues() const
{
	return isotopologues_;
}

// Return whether the specified Isotopologue exists
bool Species::hasIsotopologue(const Isotopologue* iso) const
{
	return isotopologues_.contains(iso);
}

// Generate unique Isotopologue name with base name provided
const char* Species::uniqueIsotopologueName(const char* base, const Isotopologue* exclude) const
{
	static CharString uniqueName;
	CharString baseName = base;
	Isotopologue* iso;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	if (DissolveSys::sameString(baseName, "Natural")) highest = 0;
	for (iso = isotopologues_.first(); iso != NULL; iso = iso->next())
	{
		if (iso == exclude) continue;
		if (strcmp(baseName, iso->name()) == 0) highest = 0;
		else if (strcmp(baseName,DissolveSys::beforeLastChar(iso->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(iso->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

// Search for Isotopologue by name
Isotopologue* Species::findIsotopologue(const char* name)
{
	// Check for the natural Isotopologue
	if (DissolveSys::sameString("Natural", name)) return naturalIsotopologue();

	for (Isotopologue *iso = isotopologues_.first(); iso != NULL; iso = iso->next()) if (DissolveSys::sameString(name, iso->name())) return iso;

	return NULL;
}

// Return index of specified Isotopologue
int Species::indexOfIsotopologue(const Isotopologue* iso) const
{
	return isotopologues_.indexOf(iso);
}
