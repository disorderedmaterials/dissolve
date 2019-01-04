/*
	*** Core Data
	*** src/classes/coredata.cpp
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

#include "classes/coredata.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "module/list.h"
#include "base/sysfunc.h"

// Constructor
CoreData::CoreData()
{
	moduleList_ = NULL;
}

// Destructor
CoreData::~CoreData()
{
}

/*
 * Atom Types
 */

// Add new AtomType
AtomType* CoreData::addAtomType()
{
	return atomTypes_.add();
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
const char* CoreData::uniqueAtomTypeName(const char* base, AtomType* exclude) const
{
	CharString baseName = base;
	AtomType* at;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (at = atomTypes_.first(); at != NULL; at = at->next)
	{
		if (at == exclude) continue;
		if (DissolveSys::sameString(baseName, at->name())) highest = 0;
		else if (DissolveSys::sameString(baseName,DissolveSys::beforeLastChar(at->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(at->name(), '_'));
	}

	static CharString uniqueName;
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
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
	return species_.add();
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
const char* CoreData::uniqueSpeciesName(const char* base, Species* exclude) const
{
	CharString baseName = base;
	Species* sp;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (sp = species_.first(); sp != NULL; sp = sp->next)
	{
		if ( sp == exclude) continue;
		if (DissolveSys::sameString(baseName, sp->name())) highest = 0;
		else if (DissolveSys::sameString(baseName,DissolveSys::beforeLastChar(sp->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(sp->name(), '_'));
	}

	static CharString uniqueName;
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
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
	return configurations_.add();
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
const char* CoreData::uniqueConfigurationName(const char* base, Configuration* exclude) const
{
	CharString baseName = base;
	Configuration* cfg;
	int highest = -1;
	
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find all existing names which are the same as 'baseName' up to the first '_', and get the highest appended number
	for (cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		if ( cfg == exclude) continue;
		if (DissolveSys::sameString(baseName, cfg->name())) highest = 0;
		else if (DissolveSys::sameString(baseName,DissolveSys::beforeLastChar(cfg->name(),'_')) == 0) highest = atoi(DissolveSys::afterLastChar(cfg->name(), '_'));
	}

	static CharString uniqueName;
	if (highest > -1) uniqueName.sprintf("%s_%i", baseName.get(), ++highest);
	else uniqueName = baseName;
	
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

// Set target ModuleList
void CoreData::setModuleList(ModuleList* moduleList)
{
	moduleList_ = moduleList;
}

// Search for any instance of any module with the specified unique name
Module* CoreData::findModule(const char* uniqueName) const
{
	if (!moduleList_)
	{
		printf("Error - ModuleList pointer not set in CoreData.\n");
		return NULL;
	}

	return moduleList_->findInstanceByUniqueName(uniqueName);
}
