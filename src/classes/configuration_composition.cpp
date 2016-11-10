/*
	*** Configuration - Composition 
	*** src/classes/configuration_composition.cpp
	Copyright T. Youngs 2012-2016

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
#include "classes/species.h"

// Set name of the configuration
void Configuration::setName(const char* name)
{
	name_ = name;
	
	// Generate a nice name (i.e. no spaces, slashes etc.)
	niceName_ = name_;
	niceName_.replace(' ', '_');
	niceName_.replace('/', '_');
}

// Return name of the configuration
const char* Configuration::name()
{
	return name_.get();
}



// Add Species to list of those used by the Configuration
bool Configuration::addUsedSpecies(Species* sp, double relativePopulation)
{
	// Check that we have not already added it to this Configuration
	if (usedSpecies_.contains(sp))
	{
		Messenger::error("Configuration '%s' already references Species '%s'.", name(), sp->name());
		return false;
	}

	usedSpecies_.add(sp, relativePopulation);

	return true;
}

// Return feference list of Species used by the Configuration and their relative populations
RefListItem<Species,double>* Configuration::usedSpecies()
{
	return usedSpecies_.first();
}

// Return total relative population of Species
double Configuration::totalRelative() const
{
	double total = 0.0;
	for (RefListItem<Species,double>* ri = usedSpecies_.first(); ri != NULL; ri = ri->next) total += ri->data;
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

/*
 * \brief Set the atomic density of the system (atoms/A3)
 */
void Configuration::setAtomicDensity(double density)
{
	density_ = density;
	densityIsAtomic_ = true;
}

/*
 * \brief Set the chemical density of the system (g/cm3)
 */
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

/*
 * \brief Return whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
 */
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
	for (RefListItem<Species,double>* ri = usedSpecies_.first(); ri != NULL; ri = ri->next)
	{
		mass += multiplier_*ri->data * ri->item->mass();
		nAtoms += multiplier_*ri->data * ri->item->nAtoms();
	}

	// Convert density from g/cm3 to g/A3
	double rho = nAtoms  / ((mass / AVOGADRO) / (density_ / 1.0E24));
	
	Messenger::printVerbose("Converting %f atoms (mass = %f, density = %f) to atomic density = %f\n", nAtoms, mass, density_, rho);

	return rho;
}

// Set whether a random config should be generated (as opposed to loading one from a file)
void Configuration::setRandomConfiguration(bool b)
{
	randomConfiguration_ = b;
}

// Return whether a random config should be generated (as opposed to loading one from a file)
bool Configuration::randomConfiguration()
{
	return randomConfiguration_;
}

// Set file containing initial coordinates
void Configuration::setInitialCoordinatesFile(const char* fileName)
{
	initialCoordinatesFile_ = fileName;
}

// Return file containing initial coordinates
const char* Configuration::initialCoordinatesFile()
{
	return initialCoordinatesFile_.get();
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
