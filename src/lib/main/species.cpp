/*
	*** dUQ - Species Functions
	*** src/lib/main/species.cpp
	Copyright T. Youngs 2012-2013

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

#include "main/duq.h"
#include "main/flags.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

/*
// Species Definition
*/

/*!
 * \brief Add a new Species to the list
 */
Species *DUQ::addSpecies()
{
	// Create a new Species
	Species* sp = species_.add();

	// Must update Samples...
	updateSamples();

	Flags::wave(Flags::SpeciesChanged);
	
	return sp;
}

/*!
 * \brief Remove the specified Species from the list
 */
void DUQ::removeSpecies(Species* sp)
{
	// Remove Species
	species_.remove(sp);
	
	// Update Samples
	updateSamples();

	Flags::wave(Flags::SpeciesChanged);
}

/*!
 * \brief Return number of defined Species
 */
int DUQ::nSpecies() const
{
	return species_.nItems();
}

/*!
 * \brief Return first Species in the list
 */
Species *DUQ::species() const
{
	return species_.first();
}

/*!
 * \brief Return nth Species in the list
 */
Species *DUQ::species(int n)
{
	return species_[n];
}

/*!
 * \brief Generate unique Species name with base name provided
 */
const char* DUQ::uniqueSpeciesName(const char* base, Species* exclude) const
{
	static Dnchar uniqueName;
	Dnchar baseName = base;
	Species* sp;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";
	
	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (sp = species_.first(); sp != NULL; sp = sp->next)
	{
		if (sp == exclude) continue;
		if (strcmp(baseName, sp->name()) == 0) highest = 0;
		else if (strcmp(baseName,beforeLastChar(sp->name(),'_')) == 0) highest = atoi(afterLastChar(sp->name(), '_'));
	}
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
	return uniqueName;
}

/*!
 * \brief Search for Species by name
 */
Species* DUQ::findSpecies(const char* name) const
{
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next) if (strcmp(name,sp->name()) == 0) return sp;
	return NULL;
}

/*!
 * \brief Update Species (or all) Isotopologues (or specified)
 * \details Update all Isotopologues contained by all (or a specified) Species. Should be called after, for example
 * transmuting an Atom's element through the GUI. If a Species and an Isotopologue are provided, then only this
 * Isotopologue will be updated.
 */
void DUQ::updateIsotopologues(Species* species, Isotopologue* iso)
{
	if (iso) iso->update(atomTypes_);
	else if (species) species->updateIsotopologues(atomTypes_);
	else for (species = species_.first(); species != NULL; species = species->next) species->updateIsotopologues(atomTypes_);
}

/*!
 * \brief Remove Isotopologue from Species
 * \details Removes the specified Isotopologue from the specified Species, and updates all dependent data.
 */
void DUQ::removeSpeciesIsotopologue(Species* species, Isotopologue* iso)
{
	// NULL pointer check
	if (species == NULL)
	{
		msg.error("NULL_POINTER - NULL Species passed to DUQ::removeSpeciesIsotopologue.\n");
		return;
	}
	if (iso == NULL)
	{
		msg.error("NULL_POINTER - NULL Isotopologue passed to DUQ::removeSpeciesIsotopologue.\n");
		return;
	}
	
	species->removeIsotopologue(iso);
	updateSamples();
	
	Flags::wave(Flags::IsotopologuesChanged);
}
