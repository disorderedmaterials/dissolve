/*
	*** Module Groups
	*** src/module/groups.cpp
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

#include "module/groups.h"
#include "module/group.h"
#include "base/sysfunc.h"

// Constructor
ModuleGroups::ModuleGroups()
{
}

// Destructor
ModuleGroups::~ModuleGroups()
{
}

/*
 * Module Groups
 */

// Add Module to specified group, creating it if necessary
ModuleGroup* ModuleGroups::addModule(Module* module, const char* groupName)
{
	// Does the specified group exist?
	ModuleGroup* moduleGroup;
	for (moduleGroup = groups_.first(); moduleGroup != NULL; moduleGroup = moduleGroup->next) if (DissolveSys::sameString(moduleGroup->name(), groupName)) break;
	if (moduleGroup == NULL)
	{
		moduleGroup = new ModuleGroup(groupName);
		groups_.own(moduleGroup);
	}

	allModules_.add(module);
	moduleGroup->add(module);

	return moduleGroup;
}

// Return current list of groups
const List<ModuleGroup>& ModuleGroups::groups() const
{
	return groups_;
}

// Return reflist of all Modules present over all groups
const RefList<Module,bool>& ModuleGroups::modules() const
{
	return allModules_;
}

// Number of Modules present of all groups
int ModuleGroups::nModules() const
{
	return allModules_.nItems();
}
