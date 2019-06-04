/*
	*** Configuration - Composition 
	*** src/classes/configuration_composition.cpp
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

// Add Species to list of those used by the Configuration
SpeciesInfo* Configuration::addUsedSpecies(Species* sp, double relativePopulation)
{
	// Check that we have not already added it to this Configuration
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) if (spInfo->species() == sp) 
	{
		Messenger::error("Configuration '%s' already references Species '%s'.", name(), sp->name());
		return NULL;
	}

	// Add new SpeciesInfo
	SpeciesInfo* spInfo = usedSpecies_.add();
	spInfo->setSpecies(sp);
	spInfo->setPopulation(relativePopulation);

	return spInfo;
}

// Return SpeciesInfo for specified Species
SpeciesInfo* Configuration::usedSpeciesInfo(Species* sp)
{
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) if (spInfo->species() == sp) return spInfo;

	return NULL;
}

// Return list of SpeciesInfo for the Configuration
List<SpeciesInfo>& Configuration::usedSpecies()
{
	return usedSpecies_;
}

// Return if the specified Species is present in the usedSpecies list
bool Configuration::hasUsedSpecies(Species* sp)
{
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) if (spInfo->species() == sp) return true;

	return false;
}

// Return total relative population of Species
double Configuration::totalRelative() const
{
	double total = 0.0;
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next) total += spInfo->population();
	return total;
}

// Set multiplier for System components
void Configuration::setMultiplier(int mult)
{
	multiplier_ = mult;
}

// Return multiplier for System components
int Configuration::multiplier() const
{
	return multiplier_;
}

// Set the atomic density of the system (atoms/A3)
void Configuration::setAtomicDensity(double density)
{
	density_ = density;
	densityIsAtomic_ = true;
}

// Set the chemical density of the system (g/cm3)
void Configuration::setChemicalDensity(double density)
{
	density_ = density;
	densityIsAtomic_ = false;
}

// Return the density of the system
double Configuration::density() const
{
	return density_;
}

// Return whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
bool Configuration::densityIsAtomic() const
{
	return densityIsAtomic_;
}

// Return the atomic density of the system
double Configuration::atomicDensity() const
{
	// Calculate atomic density from chemical density?
	if (densityIsAtomic_) return density_;
	
	// Determine total atomic mass and number of atoms in system
	double mass = 0.0, nAtoms = 0.0;
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next)
	{
		Species* sp = spInfo->species();

		mass += multiplier_ * spInfo->population() * sp->mass();
		nAtoms += multiplier_ * spInfo->population() * sp->nAtoms();
	}

	// Convert density from g/cm3 to g/A3
	double rho = nAtoms  / ((mass / AVOGADRO) / (density_ / 1.0E24));
	
	Messenger::printVerbose("Converting %f atoms (mass = %f, density = %f) to atomic density = %f\n", nAtoms, mass, density_, rho);

	return rho;
}

// Return import coordinates file / format
CoordinateImportFileFormat& Configuration::inputCoordinates()
{
	return inputCoordinates_;
}

// Set configuration temperature
void Configuration::setTemperature(double t)
{
	temperature_ = t;
}

// Return configuration temperature
double Configuration::temperature()
{
	return temperature_;
}
