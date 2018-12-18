/*
	*** Module List
	*** src/module/list.h
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

#ifndef DISSOLVE_MODULELIST_H
#define DISSOLVE_MODULELIST_H

#include "module/reference.h"
#include "templates/list.h"

// Forward Declarations
class Module;
class GenericList;

// ModuleList
class ModuleList
{
	public:
	// Constructor
	ModuleList();
	// Destructor
	~ModuleList();


	/*
	 * Module List
	 */
	private:
	// List of Module References
	List<ModuleReference> modules_;

	public:
	// Clear list
	void clear();
	// Associate Module to list
	Module* add(Module* module, Configuration* location = NULL, Module* addBeforeThis = NULL);
	// Find associated Module by name
	Module* find(const char* name) const;
	// Find ModuleReference for specified Module
	ModuleReference* contains(Module* module);
	// Return number of Modules in the list
	int nModules() const;
	// Return list of ModuleReferences
	List<ModuleReference>& modules();


	/*
	 * Master Module Instances
	 */
	private:
	// List of master Module instances
	List<ModuleReference> masterInstances_;
	// List of Modules that failed to register
	List<ModuleReference> failedRegistrations_;

	public:
	// Clear master Module instances
	void clearMasterInstances();
	// Register Module
	void registerMasterInstance(Module* mainInstance);
	// Return number of failed Module registrations
	int nFailedRegistrations() const;
	// Find master instance of specified Module type
	Module* findMasterInstance(const char* type) const;
	// Print out registered module information, and return false if any registration errors were encountered
	void printMasterModuleInformation() const;
	// Return list of all master instances
	const List<ModuleReference>& masterInstances() const;
	// Search for any instance of any module with the specified unique name
	Module* findInstanceByUniqueName(const char* uniqueName) const;
};

#endif

