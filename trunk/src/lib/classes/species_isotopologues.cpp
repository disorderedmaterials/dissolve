/*
	*** Species Definition - Isotopologues
	*** src/lib/classes/species_isotopologues.cpp
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/flags.h"
#include "classes/species.h"
#include <string.h>
#include <base/sysfunc.h>

/*!
 * \brief Update current Isotopologues
 */
void Species::updateIsotopologues(const List<AtomType>& atomTypes)
{
	for (Isotopologue* iso = isotopologues_.first(); iso != NULL; iso = iso->next) iso->update(atomTypes);
}

/*!
 * \brief Add a new Isotopologue to this species
 */
Isotopologue *Species::addIsotopologue(const char *baseName)
{
	Isotopologue* itp = isotopologues_.add();
	itp->setParent(this);
	itp->setName(uniqueIsotopologueName(baseName));

	Flags::wave(Flags::IsotopologuesChanged);

	return itp;
}

/*!
 * \brief Remove specified Isotopologue from this Species
 */
void Species::removeIsotopologue(Isotopologue* iso)
{
	if (iso == NULL) msg.error("NULL_POINTER - NULL Isotopologue passed to Species::removeIsotopologue().\n");
	else if (isotopologues_.contains(iso))
	{
		if (highlightedIsotopologue_ == iso) highlightedIsotopologue_ = NULL;
		Dnchar tempName = iso->name();
		isotopologues_.remove(iso);
		msg.print("Removed Isotopologue '%s' from Species '%s'.\n", tempName.get(), name_.get());
		
		Flags::wave(Flags::IsotopologuesChanged);
	}
	else
	{
		msg.print("BAD_REMOVE - Can't remove specified Isotopologue '%s' from Species '%s' since it doesn't exist.\n", iso->name(), name_.get());
		if (iso->parent() == NULL) msg.print("BAD_CLASS - No parent pointer set in Isotopologue '%s'.\n", iso->name());
		else msg.print("BAD_REMOVE - Parent Species (%s) of Isotopologue '%s' is different from this one (%s).\n", iso->parent()->name(), iso->name(), name());
	}
}

/*!
 * \brief Return number of defined Isotopologues
 */
int Species::nIsotopologues() const
{
	return isotopologues_.nItems();
}

/*!
 * \brief Return first Isotopologue defined
 */
Isotopologue *Species::isotopologues() const
{
	return isotopologues_.first();
}

/*!
 * \brief Return nth Isotopologue defined
 */
Isotopologue *Species::isotopologue(int n)
{
	return isotopologues_[n];
}

/*!
 * \brief Return whether the specified Isotopologue exists
 */
bool Species::hasIsotopologue(Isotopologue* iso) const
{
	return isotopologues_.contains(iso);
}

/*!
 * \brief Generate unique Isotopologue name with base name provided
 */
const char* Species::uniqueIsotopologueName(const char* base, Isotopologue* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	Isotopologue* iso;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (iso = isotopologues_.first(); iso != NULL; iso = iso->next)
	{
		if (iso == exclude) continue;
		if (strcmp(baseName, iso->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(iso->name(),'_')) == 0) highest = atoi(afterLastChar(iso->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Search for Isotopologue by name
 */
Isotopologue *Species::findIsotopologue(const char* name) const
{
	for (Isotopologue *iso = isotopologues_.first(); iso != NULL; iso = iso->next) if (strcmp(name,iso->name()) == 0) return iso;
	return NULL;
}

/*!
 * \brief Return index of specified Isotopologue
 */
int Species::indexOfIsotopologue(Isotopologue* iso) const
{
	return isotopologues_.indexOf(iso);
}

/*!
 * \brief Set highlighted Isotopologue
 */
void Species::setHighlightedIsotopologue(Isotopologue* iso)
{
	highlightedIsotopologue_ = iso;
}

/*!
 * \brief Return highlighted Isotopologue
 */
Isotopologue* Species::highlightedIsotopologue()
{
	return highlightedIsotopologue_;
}
