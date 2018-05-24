/*
	*** Module List
	*** src/module/list.cpp
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

#include "module/list.h"
#include "module/module.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Static Members
List<ModuleReference> ModuleList::masterInstances_;
List<ModuleReference> ModuleList::failedRegistrations_;

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
Module* ModuleList::add(Module* module, Configuration* location, Module* addBeforeThis)
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
		Module* existingModule = find(module->name());
		if (existingModule) moduleToAdd = existingModule;
		else moduleToAdd = module->createInstance();
	}
	else
	{
		// Multiple instance modules are many-per-parent, so always create a new instance
		moduleToAdd = module->createInstance();
	}

	// Add the module pointer to the list
	ModuleReference* newModuleItem;
	if (addBeforeThis)
	{
		// Find the specified Module in the list
		ModuleReference* modRef = contains(addBeforeThis);
		if (!modRef)
		{
			Messenger::error("ModuleList doesn't contain the Module pointer given as 'addBeforeThis'.\n");
			return NULL;
		}
		else newModuleItem = modules_.insertBefore(modRef);
	}
	else newModuleItem = modules_.add();
	newModuleItem->set(moduleToAdd, this, location);

	return moduleToAdd;
}

// Find associated module by name
Module* ModuleList::find(const char* name) const
{
	ListIterator<ModuleReference> moduleIterator(modules_);
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* module = modRef->module();

		if (DissolveSys::sameString(module->name(),name)) return module;
	}

	return NULL;
}

// Find ModuleReference for specified Module
ModuleReference* ModuleList::contains(Module* module)
{
	ListIterator<ModuleReference> moduleIterator(modules_);
	while (ModuleReference* modRef = moduleIterator.iterate()) if (modRef->module() == module) return modRef;

	return NULL;
}

// Return total number of Modules in the list
int ModuleList::nModules() const
{
	return modules_.nItems();
}

// Return Modules associated to Configuration
List<ModuleReference>& ModuleList::modules()
{
	return modules_;
}

/*
 * Master Module List
 */

// Register master Module isntance
void ModuleList::registerMasterInstance(Module* mainInstance)
{
	// Do sanity check on name
	ListIterator<ModuleReference> moduleIterator(masterInstances_);
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* module = modRef->module();

		if (DissolveSys::sameString(module->name(), mainInstance->name()))
		{
			Messenger::error("Two modules cannot have the same name (%s).\n", module->name());
			ModuleReference* failedRef = failedRegistrations_.add();
			failedRef->set(module, NULL, NULL);
			return;
		}
	}

	ModuleReference* masterRef = masterInstances_.add();
	masterRef->set(mainInstance, NULL, NULL);
}

// Find master instance of named Module
Module* ModuleList::findMasterInstance(const char* name)
{
	ListIterator<ModuleReference> moduleIterator(masterInstances_);
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* masterInstance = modRef->module();
		
		if (DissolveSys::sameString(masterInstance->name(), name)) return masterInstance;
	}

	return NULL;
}

// Print out registered module information
bool ModuleList::printMasterModuleInformation()
{
	Messenger::print("Module Information (%i available):\n", masterInstances_.nItems());
	ListIterator<ModuleReference> moduleIterator(masterInstances_);
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* masterInstance = modRef->module();

		Messenger::print(" --> %s\n", masterInstance->name());
		Messenger::print("     %s\n", masterInstance->brief());
	}

	if (failedRegistrations_.nItems() > 0)
	{
		Messenger::print("\n");
		Messenger::print("Failed module registrations (%i):\n\n", failedRegistrations_.nItems());
		ListIterator<ModuleReference> moduleIterator(failedRegistrations_);
		while (ModuleReference* modRef = moduleIterator.iterate())
		{
			Module* failedInstance = modRef->module();

			Messenger::print(" --> %s\n", failedInstance->name());
		}
	}

	return (failedRegistrations_.nItems() == 0);
}

// Return list of all master instances
List<ModuleReference>& ModuleList::masterInstances()
{
	return masterInstances_;
}

// Search for any instance of any module with the specified unique name
Module* ModuleList::findInstanceByUniqueName(const char* uniqueName)
{
	ListIterator<ModuleReference> moduleIterator(masterInstances_);
	while (ModuleReference* modRef = moduleIterator.iterate())
	{
		Module* masterInstance = modRef->module();

		// Master instance itself?
		if (DissolveSys::sameString(masterInstance->uniqueName(), uniqueName)) return masterInstance;

		// Child instances
		Module* instance = masterInstance->findInstance(uniqueName);
		if (instance) return instance;
	}

	return NULL;
}
