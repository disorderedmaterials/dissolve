/*
	*** dUQ Module List
	*** src/modules/modulelist.h
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

#ifndef DUQ_MODULELIST_H
#define DUQ_MODULELIST_H

#include "base/variablelist.h"
#include "templates/reflist.h"

// Forward Declarations
class Module;

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
	// List of Modules
	RefList<Module,bool> modules_;
	// Variables set by Modules
	VariableList moduleVariables_;

	public:
	// Associate Module to list
	Module* addModule(Module* module, bool autoAddDependents = false, RefListItem<Module, bool>* addBeforeThis = NULL);
	// Find associated Module by name
	Module* findModule(const char* name);
	// Return number of Modules associated
	int nModules();
	// Return Modules associated to list
	RefList<Module,bool>& modules();
	// Set Module variable
	void setModuleVariable(const char* name, VariableValue value, const char* description, const char* source);
	// Append Module variable
	void appendModuleVariable(const char* name, VariableValue value, const char* description, const char* source);
	// Return named Module variable
	Variable* moduleVariable(const char* name, const char* source);


	/*
	 * Master Module Instances
	 */
	private:
	// List of master Module instances
	static RefList<Module,bool> masterInstances_;
	// List of Modules that failed to register
	static RefList<Module,bool> failedRegistrations_;

	public:
	// Register Module
	static void registerMasterInstance(Module* mainInstance);
	// Find master instance of named Module
	static Module* findMasterInstance(const char* name);
	// Print out registered module information, and return false if any registration errors were encountered
	static bool printMasterModuleInformation();
	// Return list of all master instances
	static RefList<Module,bool>& masterInstances();
};

#endif

