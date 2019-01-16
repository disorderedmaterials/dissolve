/*
	*** Core Data
	*** src/classes/coredata.h
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

#ifndef DISSOLVE_COREDATA_H
#define DISSOLVE_COREDATA_H

#include "templates/list.h"

// Forward Declarations
class AtomType;
class Configuration;
class Element;
class Species;
class Module;

// Core Data Container
class CoreData
{
	public:
	// Constructor / Destructor
	CoreData();
	~CoreData();


	/*
	 * Atom Types
	 */
	private:
	// Core AtomTypes list
	List<AtomType> atomTypes_;

	public:
	// Add new AtomType
	AtomType* addAtomType();
	// Return number of AtomTypes in list
	int nAtomTypes() const;
	// Return core AtomTypes list
	List<AtomType>& atomTypes();
	// Return core AtomTypes list (const)
	const List<AtomType>& constAtomTypes() const;
	// Return nth AtomType in list
	AtomType* atomType(int n);
	// Generate unique AtomType name with base name provided
	const char* uniqueAtomTypeName(const char* baseName) const;
	// Search for AtomType by name
	AtomType* findAtomType(const char* name) const;


	/*
	 * Species
	 */
	private:
	// Core Species list
	List<Species> species_;

	public:
	// Add new Species
	Species* addSpecies();
	// Return number of Species in list
	int nSpecies() const;
	// Return core Species list
	List<Species>& species();
	// Return core Species list (const)
	const List<Species>& constSpecies() const;
	// Return nth Species in list
	Species* species(int n);
	// Generate unique Species name with base name provided
	const char* uniqueSpeciesName(const char* baseName) const;
	// Search for Species by name
	Species* findSpecies(const char* name) const;


	/*
	 * Configurations
	 */
	private:
	// Core Configurations list
	List<Configuration> configurations_;

	public:
	// Add new Configuration
	Configuration* addConfiguration();
	// Return number of Configuration in list
	int nConfigurations() const;
	// Return core Configuration list
	List<Configuration>& configurations();
	// Return core Configuration list (const)
	const List<Configuration>& constConfigurations() const;
	// Return nth Configuration in list
	Configuration* configuration(int n);
	// Generate unique Configuration name with base name provided
	const char* uniqueConfigurationName(const char* baseName) const;
	// Search for Configuration by name
	Configuration* findConfiguration(const char* name) const;


	/*
	 * Module Instances
	 */
	private:
	// Pointer to Module instances list
	List<Module>* moduleInstances_;

	public:
	// Set target Module instances list
	void setModuleInstances(List<Module>* moduleInstances);
	// Search for any instance of any module with the specified unique name
	Module* findModule(const char* uniqueName) const;
};

#endif
