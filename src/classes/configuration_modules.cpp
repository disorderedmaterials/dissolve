/*
	*** Configuration - Modules
	*** src/classes/configuration_modules.cpp
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

#include "classes/configuration.h"
#include "modules/module.h"
#include <string.h>

// Add module to Configuration
Module* Configuration::addModule(Module* module)
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
		// Single instance modules are one-per-Configuration, so must see if it is already in the relevant list...
		Module* existingModule = findModule(module->name());
		if (existingModule) moduleToAdd = existingModule;
		else moduleToAdd = module->createInstance();
	}
	else
	{
		// Multiple instance modules are many-per-Configuration, so always create a new instance
		moduleToAdd = module->createInstance();
	}

	// Add the module pointer to the list
	modules_.add(moduleToAdd);

	// Add our pointer to the Module's list of associated Configurations
	if (!moduleToAdd->addConfigurationTarget(this)) return NULL;

	return moduleToAdd;
}

// Find associated module by name
Module* Configuration::findModule(const char* name)
{
	RefListIterator<Module,bool> moduleIterator(modules_);
	while (Module* module = moduleIterator.iterate()) if (strcmp(module->name(),name) == 0) return module;

	return NULL;
}

// Return total number of Modules associated
int Configuration::nModules()
{
	return modules_.nItems();
}

// Return Modules associated to Configuration
RefList<Module,bool>& Configuration::modules()
{
	return modules_;
}

// Set Module variable
void Configuration::setModuleVariable(const char* name, VariableValue value, const char* description, const char* source)
{
	moduleVariables_.setVariable(name, value, description, source);
}

// Append Module variable
void Configuration::appendModuleVariable(const char* name, VariableValue value, const char* description, const char* source)
{
	moduleVariables_.appendVariable(name, value, description, source);
}

// Return named Module variable
Variable* Configuration::moduleVariable(const char* name, const char* source)
{
	return moduleVariables_.variable(name, source);
}

