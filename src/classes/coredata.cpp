/*
	*** Core Data
	*** src/classes/coredata.cpp
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

#include "classes/coredata.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "module/list.h"
#include "module/module.h"
#include "base/sysfunc.h"

// Constructor
CoreData::CoreData()
{
	moduleInstances_ = NULL;
	inputFilename_ = NULL;
}

// Destructor
CoreData::~CoreData()
{
}

// Clear all data
void CoreData::clear()
{
	configurations_.clear();
	species_.clear();
	masterBonds_.clear();
	masterAngles_.clear();
	masterTorsions_.clear();
	atomTypes_.clear();
}

/*
 * Atom Types
 */

// Add new AtomType
AtomType* CoreData::addAtomType(Element* el)
{
	AtomType* newAtomType = atomTypes_.add();

	// Create a suitable unique name
	newAtomType->setName(uniqueAtomTypeName(el->symbol()));

	// Set data
	newAtomType->setElement(el);
	newAtomType->setIndex(nAtomTypes() - 1);

	// Bump version
	++atomTypesVersion_;

	return newAtomType;
}

// Remove specified AtomType
void CoreData::removeAtomType(AtomType* at)
{
	atomTypes_.remove(at);
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
	for (AtomType* at = atomTypes_.first(); at != NULL; at = at->next()) if (DissolveSys::sameString(at->name(),name)) return at;

	return NULL;
}

// Bump AtomTypes version
void CoreData::bumpAtomTypesVersion()
{
	++atomTypesVersion_;
}

// Return AtomTypes version
int CoreData::atomTypesVersion() const
{
	return atomTypesVersion_;
}

/*
 * Master Intramolecular Terms
 */

// Add new master Bond parameters
MasterIntra* CoreData::addMasterBond(const char* name)
{
	// Check for existence of master Bond already
	if (hasMasterBond(name))
	{
		Messenger::error("Refused to add a new master Bond named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Bond
	MasterIntra* b = masterBonds_.add();
	b->setName(name);
	b->setType(SpeciesIntra::BondInteraction);

	return b;
}

// Return number of master Bond parameters in list
int CoreData::nMasterBonds() const
{
	return masterBonds_.nItems();
}

// Return list of master Bond parameters
const List<MasterIntra>& CoreData::masterBonds() const
{
	return masterBonds_;
}

// Return nth master Bond parameters
MasterIntra* CoreData::masterBond(int n)
{
	return masterBonds_[n];
}

// Return whether named master Bond parameters exist
MasterIntra* CoreData::hasMasterBond(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next()) if (DissolveSys::sameString(trimmedName, b->name())) return b;
	return NULL;
}

// Add new master Angle parameters
MasterIntra* CoreData::addMasterAngle(const char* name)
{
	// Check for existence of master Angle already
	if (hasMasterAngle(name))
	{
		Messenger::error("Refused to add a new master Angle named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Angle
	MasterIntra* a = masterAngles_.add();
	a->setName(name);
	a->setType(SpeciesIntra::AngleInteraction);

	return a;
}

// Return number of master Angle parameters in list
int CoreData::nMasterAngles() const
{
	return masterAngles_.nItems();
}

// Return list of master Angle parameters
const List<MasterIntra>& CoreData::masterAngles() const
{
	return masterAngles_;
}

// Return nth master Angle parameters
MasterIntra* CoreData::masterAngle(int n)
{
	return masterAngles_[n];
}

// Return whether named master Angle parameters exist
MasterIntra* CoreData::hasMasterAngle(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next()) if (DissolveSys::sameString(trimmedName, a->name())) return a;
	return NULL;
}

// Add new master Torsion parameters
MasterIntra* CoreData::addMasterTorsion(const char* name)
{
	// Check for existence of master Torsion already
	if (hasMasterTorsion(name))
	{
		Messenger::error("Refused to add a new master Torsion named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Torsion
	MasterIntra* t = masterTorsions_.add();
	t->setName(name);
	t->setType(SpeciesIntra::TorsionInteraction);

	return t;
}

// Return number of master Torsion parameters in list
int CoreData::nMasterTorsions() const
{
	return masterTorsions_.nItems();
}

// Return list of master Torsion parameters
const List<MasterIntra>& CoreData::masterTorsions() const
{
	return masterTorsions_;
}

// Return nth master Torsion parameters
MasterIntra* CoreData::masterTorsion(int n)
{
	return masterTorsions_[n];
}

// Return whether named master Torsion parameters exist
MasterIntra* CoreData::hasMasterTorsion(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next()) if (DissolveSys::sameString(trimmedName, t->name())) return t;
	return NULL;
}

// Add new master Improper parameters
MasterIntra* CoreData::addMasterImproper(const char* name)
{
	// Check for existence of master Improper already
	if (hasMasterImproper(name))
	{
		Messenger::error("Refused to add a new master Improper named '%s' since one with the same name already exists.\n", name);
		return NULL;
	}

	// OK to add new master Improper
	MasterIntra* i = masterImpropers_.add();
	i->setName(name);
	i->setType(SpeciesIntra::ImproperInteraction);

	return i;
}

// Return number of master Improper parameters in list
int CoreData::nMasterImpropers() const
{
	return masterImpropers_.nItems();
}

// Return list of master Improper parameters
const List<MasterIntra>& CoreData::masterImpropers() const
{
	return masterImpropers_;
}

// Return nth master Improper parameters
MasterIntra* CoreData::masterImproper(int n)
{
	return masterImpropers_[n];
}

// Return whether named master Improper parameters exist
MasterIntra* CoreData::hasMasterImproper(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* t = masterImpropers_.first(); t != NULL; t = t->next()) if (DissolveSys::sameString(trimmedName, t->name())) return t;
	return NULL;
}

// Return the named master term (of any form) if it exists
MasterIntra* CoreData::findMasterTerm(const char* name) const
{
	// Remove leading '@' if necessary
	const char* trimmedName = name[0] == '@' ? &name[1] : name;

	for (MasterIntra* b = masterBonds_.first(); b != NULL; b = b->next()) if (DissolveSys::sameString(trimmedName, b->name())) return b;
	for (MasterIntra* a = masterAngles_.first(); a != NULL; a = a->next()) if (DissolveSys::sameString(trimmedName, a->name())) return a;
	for (MasterIntra* t = masterTorsions_.first(); t != NULL; t = t->next()) if (DissolveSys::sameString(trimmedName, t->name())) return t;
	for (MasterIntra* i = masterImpropers_.first(); i != NULL; i = i->next()) if (DissolveSys::sameString(trimmedName, i->name())) return i;

	return NULL;
}

// Clear all master terms
void CoreData::clearMasterTerms()
{
	masterBonds_.clear();
	masterAngles_.clear();
	masterTorsions_.clear();
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

// Remove specified Species
void CoreData::removeSpecies(Species* sp)
{
	species_.remove(sp);
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
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next()) if (DissolveSys::sameString(sp->name(),name)) return sp;

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

// Remove specified Configuration
void CoreData::removeConfiguration(Configuration* cfg)
{
	configurations_.remove(cfg);
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
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next()) if (DissolveSys::sameString(cfg->name(),name)) return cfg;

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

	for(auto module : *moduleInstances_) if (DissolveSys::sameString(module->uniqueName(), uniqueName)) return module;

	return NULL;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(const char* moduleType) const
{
	RefList<Module> modules;

	for(auto module : *moduleInstances_) if (DissolveSys::sameString(module->type(), moduleType)) modules.append(module);

	return modules;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(const CharStringList& moduleTypes) const
{
	RefList<Module> modules;

	for(auto module : *moduleInstances_) if (moduleTypes.contains(module->type())) modules.append(module);

	return modules;
}

/*
 * Input Filename
 */

// Set pointer to the current input filename
void CoreData::setInputFilename(const CharString* inputFilePtr)
{
	inputFilename_ = inputFilePtr;
}

// Return the current input filename (from Dissolve)
const char* CoreData::inputFilename() const
{
	return (inputFilename_ ? inputFilename_->get() : "NO_INPUTFILENAME_SET");
}
