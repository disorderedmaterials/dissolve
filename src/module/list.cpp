/*
	*** dUQ Module List
	*** src/module/list.cpp
	Copyright T. Youngs 2012-2017

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

#include "module/list.h"
#include "module/module.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Static Members
RefList<Module,bool> ModuleList::masterInstances_;
RefList<Module,bool> ModuleList::failedRegistrations_;

// Constructor
ModuleList::ModuleList()
{
}

// Destructor
ModuleList::~ModuleList()
{
}

/*
 * Module List
 */

// Add module to list
Module* ModuleList::addModule(Module* module, GenericList& moduleData, Module* addBeforeThis)
{
	Module* moduleToAdd = NULL;

	// Check the module instance type before adding it to the relevant list
	if (module->instanceType() == Module::UniqueInstance)
	{
		// Unique modules always re-use the master instance (provided as the argument)
		moduleToAdd = module;
	}
	else if (module->instanceType() == Module::SingleInstance)
	{
		// Single instance modules are one-per-parent, so must see if it is already in the relevant list...
		Module* existingModule = findModule(module->name());
		if (existingModule) moduleToAdd = existingModule;
		else moduleToAdd = module->createInstance();
	}
	else
	{
		// Multiple instance modules are many-per-parent, so always create a new instance
		moduleToAdd = module->createInstance();
	}

	// Add the module pointer to the list
	RefListItem<Module, bool>* newModuleItem;
	if (addBeforeThis)
	{
		// Find the specified Module in the list
		RefListItem<Module,bool>* ri = modules_.contains(addBeforeThis);
		if (!ri)
		{
			Messenger::error("ModuleList doesn't contain the Module pointer given as 'addBeforeThis'.\n");
			return NULL;
		}
		else newModuleItem = modules_.addBefore(ri, moduleToAdd);
	}
	else newModuleItem = modules_.add(moduleToAdd);

	return moduleToAdd;
}

// Find associated module by name
Module* ModuleList::findModule(const char* name) const
{
	RefListIterator<Module,bool> moduleIterator(modules_);
	while (Module* module = moduleIterator.iterate()) if (DUQSys::sameString(module->name(),name)) return module;

	return NULL;
}

// Return total number of Modules in the list
int ModuleList::nModules() const
{
	return modules_.nItems();
}

// Return Modules associated to Configuration
RefList<Module,bool>& ModuleList::modules()
{
	return modules_;
}

/*
 * Reaping
 */

// Reap data from disk for all Modules
void ModuleList::reapAllData()
{
	// Loop over all Modules, loading in all the data that they can
	RefListIterator<Module,bool> allModules(modules_);
	while (Module* module = allModules.iterate()) module->reapAllInstances();
}

/*
 * Master Module List
 */

// Register master Module isntance
void ModuleList::registerMasterInstance(Module* mainInstance)
{
	// Do sanity check on names
	RefListIterator<Module,bool> moduleIterator(masterInstances_);
	while (Module* module = moduleIterator.iterate())
	{
		if (DUQSys::sameString(module->name(), mainInstance->name()))
		{
			Messenger::error("Two modules cannot have the same name (%s).\n", module->name());
			failedRegistrations_.add(module);
			return;
		}
	}

	masterInstances_.add(mainInstance);
}

// Find master instance of named Module
Module* ModuleList::findMasterInstance(const char* name)
{
	RefListIterator<Module,bool> moduleIterator(masterInstances_);
	while (Module* masterInstance = moduleIterator.iterate()) if (DUQSys::sameString(masterInstance->name(), name)) return masterInstance;

	return NULL;
}

// Print out registered module information
bool ModuleList::printMasterModuleInformation()
{
	Messenger::print("Module Information (%i available):\n", masterInstances_.nItems());
	RefListIterator<Module,bool> moduleIterator(masterInstances_);
	while (Module* masterInstance = moduleIterator.iterate())
	{
		Messenger::print(" --> %s\n", masterInstance->name());
		Messenger::print("     %s\n", masterInstance->brief());
	}

	if (failedRegistrations_.nItems() > 0)
	{
		Messenger::print("\n");
		Messenger::print("Failed module registrations (%i):\n\n", failedRegistrations_.nItems());
		RefListIterator<Module,bool> moduleIterator(failedRegistrations_);
		while (Module* failedInstance = moduleIterator.iterate()) Messenger::print(" --> %s\n", failedInstance->name());
	}

	return (failedRegistrations_.nItems() == 0);
}

// Return list of all master instances
RefList<Module,bool>& ModuleList::masterInstances()
{
	return masterInstances_;
}

// Search for any instance of any module with the specified unique name
Module* ModuleList::findInstanceByUniqueName(const char* uniqueName)
{
	RefListIterator<Module,bool> moduleIterator(masterInstances_);
	while (Module* masterInstance = moduleIterator.iterate())
	{
		// Master instance itself?
		if (DUQSys::sameString(masterInstance->uniqueName(), uniqueName)) return masterInstance;

		// Child instances
		Module* instance = masterInstance->findInstance(uniqueName);
		if (instance) return instance;
	}

	return NULL;
}
