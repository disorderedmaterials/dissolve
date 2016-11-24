/*
	*** dUQ Module List
	*** src/modules/modulelist.cpp
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

#include "modules/modulelist.h"
#include "modules/module.h"

#include "base/sysfunc.h"

/*
 * Module Registration
 * ===================
 * All modules to be registered for use in the code must be placed here.
 */
// #include "modules/atomshake.h"
// ModuleRegistrar<AtomShake> atomShakeRegistrar;
// #include "modules/energy.h"
// ModuleRegistrar<Energy> energyRegistrar;
// #include "modules/export.h"
// ModuleRegistrar<Export> exportRegistrar;
// #include "modules/partials.h"
// ModuleRegistrar<Partials> partailsRegistrar;
// #include "modules/structurefactor.h"
// ModuleRegistrar<StructureFactor> structureFactorRegistrar;
// #include "modules/test.h"
// ModuleRegistrar<Test> testRegistrar;

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
Module* ModuleList::addModule(Module* module)
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
		Module* existingModule = findMasterInstance(module->name());
		if (existingModule) moduleToAdd = existingModule;
		else moduleToAdd = module->createInstance();
	}
	else
	{
		// Multiple instance modules are many-per-parent, so always create a new instance
		moduleToAdd = module->createInstance();
	}

	// Add the module pointer to the list
	modules_.add(moduleToAdd);

	// Check dependencies of the new Module - loop over dependent Module names
	LineParser moduleParser;
	moduleParser.getArgsDelim(LineParser::Defaults, module->dependentModules());
	for (int n=0; n<moduleParser.nArgs(); ++n)
	{
		// First, make sure this is an actual Module name
		Module* dependentModule = findMasterInstance(moduleParser.argc(n));
		if (!dependentModule)
		{
			Messenger::error("Module lists a dependent module '%s', but this Module doesn't exist.\n", moduleParser.argc(n));
			return NULL;
		}

		// Find dependentModule in the previously-defined list of Modules for this Configuration
		Module* existingModule;
		RefListIterator<Module,bool> moduleIterator(modules_);
		while (existingModule = moduleIterator.iterate()) if (DUQSys::sameString(existingModule->name(),dependentModule->name())) break;
		if (existingModule)
		{
			module->addDependentModule(existingModule);
			Messenger::print("Added dependent Module '%s' to Module '%s'.\n", existingModule->uniqueName(), module->uniqueName());
		}
		else
		{
			// No Module exists in the Configuration already
			Messenger::error("The Module '%s' requires the Module '%s' to run prior to it, but the '%s' Module has not been associated to the Configuration.\n", module->name(), dependentModule->name(), dependentModule->name());
			return NULL;
		}
	}

	return moduleToAdd;
}

// Find associated module by name
Module* ModuleList::findModule(const char* name)
{
	RefListIterator<Module,bool> moduleIterator(modules_);
	while (Module* module = moduleIterator.iterate()) if (DUQSys::sameString(module->name(),name)) return module;

	return NULL;
}

// Return total number of Modules associated
int ModuleList::nModules()
{
	return modules_.nItems();
}

// Return Modules associated to Configuration
RefList<Module,bool>& ModuleList::modules()
{
	return modules_;
}

// Set Module variable
void ModuleList::setModuleVariable(const char* name, VariableValue value, const char* description, const char* source)
{
	moduleVariables_.setVariable(name, value, description, source);
}

// Append Module variable
void ModuleList::appendModuleVariable(const char* name, VariableValue value, const char* description, const char* source)
{
	moduleVariables_.appendVariable(name, value, description, source);
}

// Return named Module variable
Variable* ModuleList::moduleVariable(const char* name, const char* source)
{
	return moduleVariables_.variable(name, source);
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
