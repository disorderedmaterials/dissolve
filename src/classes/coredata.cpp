/*
	*** Core Data
	*** src/classes/coredata.cpp
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

#include "classes/coredata.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "module/list.h"
#include "module/module.h"
#include "base/sysfunc.h"

// Constructor
CoreData::CoreData()
{
	moduleInstances_ = NULL;
}

// Destructor
CoreData::~CoreData()
{
}

/*
 * Atom Types
 */

// Add new AtomType
AtomType* CoreData::addAtomType(Element* el)
{
	AtomType* newAtomType = atomTypes_.add();

	// Create a suitable unique name
	newAtomType->setName(uniqueAtomTypeName(el->name()));

	// Set data
	newAtomType->setElement(el);
	newAtomType->setIndex(nAtomTypes() - 1);

	return newAtomType;
}

// Return number of AtomTypes in list
int CoreData::nAtomTypes() const
{
	return atomTypes_.nItems();
}

// Return core AtomTypes list
List<AtomType>& CoreData::atomTypes()
{
	return atomTypes_;
}

// Return core AtomTypes list (const)
const List<AtomType>& CoreData::constAtomTypes() const
{
	return atomTypes_;
}

// Return nth AtomType in list
AtomType* CoreData::atomType(int n)
{
	return atomTypes_[n];
}

// Generate unique AtomType name with base name provided
const char* CoreData::uniqueAtomTypeName(const char* base) const
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findAtomType(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

// Search for AtomType by name
AtomType* CoreData::findAtomType(const char* name) const
{
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next) if (DissolveSys::sameString(at->name(),name)) return at;

	return NULL;
}

/*
 * Species
 */

// Add new Species
Species* CoreData::addSpecies()
{
	Species* newSpecies = species_.add();

	// Create a suitable unique name
	newSpecies->setName(uniqueSpeciesName("NewSpecies"));

	return newSpecies;
}

// Return number of Species in list
int CoreData::nSpecies() const
{
	return species_.nItems();
}

// Return core Species list
List<Species>& CoreData::species()
{
	return species_;
}

// Return core Species list (const)
const List<Species>& CoreData::constSpecies() const
{
	return species_;
}

// Return nth Species in list
Species* CoreData::species(int n)
{
	return species_[n];
}

// Generate unique Species name with base name provided
const char* CoreData::uniqueSpeciesName(const char* base) const
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findSpecies(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

// Search for Species by name
Species* CoreData::findSpecies(const char* name) const
{
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next) if (DissolveSys::sameString(sp->name(),name)) return sp;

	return NULL;
}

/*
 * Configuration
 */

// Add new Configuration
Configuration* CoreData::addConfiguration()
{
	Configuration* newConfiguration = configurations_.add();

	// Create a suitable unique name
	newConfiguration->setName(uniqueConfigurationName("NewConfiguration"));

	return newConfiguration;
}

// Return number of Configurations in list
int CoreData::nConfigurations() const
{
	return configurations_.nItems();
}

// Return core Configurations list
List<Configuration>& CoreData::configurations()
{
	return configurations_;
}

// Return core Configuration list (const)
const List<Configuration>& CoreData::constConfigurations() const
{
	return configurations_;
}

// Return nth Configuration in list
Configuration* CoreData::configuration(int n)
{
	return configurations_[n];
}

// Generate unique Configuration name with base name provided
const char* CoreData::uniqueConfigurationName(const char* base) const
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findConfiguration(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

// Search for Configuration by name
Configuration* CoreData::findConfiguration(const char* name) const
{
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next) if (DissolveSys::sameString(cfg->name(),name)) return cfg;

	return NULL;
}

/*
 * Module List
 */

// Set target Module instances list
void CoreData::setModuleInstances(RefList<Module>* moduleInstances)
{
	moduleInstances_ = moduleInstances;
}

// Search for any instance of any module with the specified unique name
Module* CoreData::findModule(const char* uniqueName) const
{
	if (!moduleInstances_)
	{
		printf("Error - RefList<Module> pointer not set in CoreData.\n");
		return NULL;
	}

	RefListIterator<Module> moduleIterator(*moduleInstances_);
	while (Module* module = moduleIterator.iterate()) if (DissolveSys::sameString(module->uniqueName(), uniqueName)) return module;

	return NULL;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(const char* moduleType) const
{
	RefList<Module> modules;

	RefListIterator<Module> moduleIterator(*moduleInstances_);
	while (Module* module = moduleIterator.iterate()) if (DissolveSys::sameString(module->type(), moduleType)) modules.append(module);

	return modules;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(const CharStringList& moduleTypes) const
{
	RefList<Module> modules;

	RefListIterator<Module> moduleIterator(*moduleInstances_);
	while (Module* module = moduleIterator.iterate()) if (moduleTypes.contains(module->type())) modules.append(module);

	return modules;
}
