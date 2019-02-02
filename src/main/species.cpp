/*
	*** Dissolve - Species Functions
	*** src/main/species.cpp
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

#include "main/dissolve.h"
#include "classes/species.h"
#include "base/sysfunc.h"
#include <string.h>

// Add a new Species to the list
Species* Dissolve::addSpecies()
{
	Species* newSpecies = coreData_.addSpecies();

	setUp_ = false;

	return newSpecies;
}

// Remove the specified Species from the list
void Dissolve::removeSpecies(Species* sp)
{
	// Remove Species
	coreData_.species().remove(sp);

	setUp_ = false;
}

// Return number of defined Species
int Dissolve::nSpecies() const
{
	return coreData_.nSpecies();
}

// Return Species list
List<Species>& Dissolve::species()
{
	return coreData_.species();
}

// Return nth Species in the list
Species* Dissolve::species(int n)
{
	return coreData_.species(n);
}

// Search for Species by name
Species* Dissolve::findSpecies(const char* name) const
{
	return coreData_.findSpecies(name);
}

// Update Species (or all) Isotopologues (or specified)
void Dissolve::updateIsotopologues(Species* species, Isotopologue* iso)
{
	if (iso) iso->update(coreData_.atomTypes());
	else if (species) species->updateIsotopologues(coreData_.atomTypes());
	else for (species = coreData_.species().first(); species != NULL; species = species->next) species->updateIsotopologues(coreData_.atomTypes());
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
