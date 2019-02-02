/*
	*** Module List
	*** src/module/list.cpp
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

#include "module/list.h"
#include "module/module.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Constructor
ModuleList::ModuleList()
{
}

// Destructor
ModuleList::~ModuleList()
{
}

// Conversion operator (List<Module>&)
ModuleList::operator List<Module>&()
{
	return modules_;
}

/*
 * Module List
 */

// Clear list
void ModuleList::clear()
{
	modules_.clear();
}

// Add module to list
bool ModuleList::add(Module* module, Module* addBeforeThis)
{
	// Add the module pointer to the list
	if (addBeforeThis)
	{
		// Find the specified Module in the list
		if (!contains(addBeforeThis))
		{
			Messenger::error("ModuleList doesn't contain the Module pointer given as 'addBeforeThis'.\n");
			return false;
		}
		else modules_.ownBefore(module, addBeforeThis);
	}
	else modules_.own(module);

	return true;
}

// Find associated module by name
Module* ModuleList::find(const char* name) const
{
	ListIterator<Module> moduleIterator(modules_);
	while (Module* module = moduleIterator.iterate()) if (DissolveSys::sameString(module->type(), name)) return module;

	return NULL;
}

// Return whether specified Module is present in the list
bool ModuleList::contains(Module* searchModule) const
{
	ListIterator<Module> moduleIterator(modules_);
	while (Module* module = moduleIterator.iterate()) if (module == searchModule) return true;

	return false;
}

// Return total number of Modules in the list
int ModuleList::nModules() const
{
	return modules_.nItems();
}

// Return list of Modules
List<Module>& ModuleList::modules()
{
	return modules_;
}
