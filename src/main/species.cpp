/*
	*** Dissolve - Species Functions
	*** src/main/species.cpp
	Copyright T. Youngs 2012-2018

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

#include "main/dissolve.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Add a new Species to the list
Species* Dissolve::addSpecies()
{
	// Create a new Species
	Species* sp = species_.add();

	setUp_ = false;

	return sp;
}

// Remove the specified Species from the list
void Dissolve::removeSpecies(Species* sp)
{
	// Remove Species
	species_.remove(sp);

	setUp_ = false;
}

// Return number of defined Species
int Dissolve::nSpecies() const
{
	return species_.nItems();
}

// Return Species list
List<Species>& Dissolve::species()
{
	return species_;
}

// Return nth Species in the list
Species* Dissolve::species(int n)
{
	return species_[n];
}

// Search for Species by name
Species* Dissolve::findSpecies(const char* name) const
{
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next) if (strcmp(name,sp->name()) == 0) return sp;
	return NULL;
}

// Update Species (or all) Isotopologues (or specified)
void Dissolve::updateIsotopologues(Species* species, Isotopologue* iso)
{
	if (iso) iso->update(atomTypes_);
	else if (species) species->updateIsotopologues(atomTypes_);
	else for (species = species_.first(); species != NULL; species = species->next) species->updateIsotopologues(atomTypes_);
}

// Remove Isotopologue from Species
void Dissolve::removeSpeciesIsotopologue(Species* species, Isotopologue* iso)
{
	// NULL pointer check
	if (species == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Species passed to Dissolve::removeSpeciesIsotopologue.\n");
		return;
	}
	if (iso == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Isotopologue passed to Dissolve::removeSpeciesIsotopologue.\n");
		return;
	}
	
	species->removeIsotopologue(iso);

	setUp_ = false;
}
