/*
	*** Module Groups
	*** src/module/groups.cpp
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
 * Module Types
 */

// Add allowed Module type
void ModuleGroups::addAllowedModuleType(const char* moduleType)
{
	// Check if the provided type is already present...
	if (moduleTypeIsAllowed(moduleType)) return;

	allowedModuleTypes_.add(moduleType);
}

// Set list of allowed Module types
void ModuleGroups::setAllowedModuleTypes(const CharStringList& moduleTypes)
{
	// Clear existing list
	allowedModuleTypes_.clear();

	// Add types from source list by hand, rather than copying, so we prune out duplicates
	for (int n=0; n<moduleTypes.nItems(); ++n) addAllowedModuleType(moduleTypes.at(n));
}

// Return if specified Module type is allowed in any group
bool ModuleGroups::moduleTypeIsAllowed(const char* moduleType) const
{
	return allowedModuleTypes_.contains(moduleType);
}

// Return list of allowed Module types
const CharStringList& ModuleGroups::allowedModuleTypes() const
{
	return allowedModuleTypes_;
}

/*
 * Module Groups
 */

// Add Module to specified group, creating it if necessary
ModuleGroup* ModuleGroups::addModule(Module* module, const char* groupName)
{
	// Does the specified group exist?
	ModuleGroup* moduleGroup;
	for (moduleGroup = groups_.first(); moduleGroup != NULL; moduleGroup = moduleGroup->next()) if (DissolveSys::sameString(moduleGroup->name(), groupName)) break;
	if (moduleGroup == NULL)
	{
		moduleGroup = new ModuleGroup(groupName);
		groups_.own(moduleGroup);
	}

	// Is the Module already in the list and assigned to a group?
	RefDataItem<Module,ModuleGroup*>* moduleItem = allModules_.contains(module);
	if (moduleItem) moduleItem->data()->remove(module);
	else allModules_.append(module, moduleGroup);

	// Add the Module to its new group
	moduleGroup->add(module);

	return moduleGroup;
}

// Remove Module
void ModuleGroups::removeModule(Module* module)
{
	// Find the Module in our list
	RefDataItem<Module,ModuleGroup*>* moduleItem = allModules_.contains(module);
	if (!moduleItem) return;

	ModuleGroup* group = moduleItem->data();

	group->remove(module);
	allModules_.remove(module);

	// Is the group now empty?
	if (group->nModules() == 0) groups_.remove(group);
}

// Number of Modules present of all groups
int ModuleGroups::nModules() const
{
	return allModules_.nItems();
}

// Return current list of groups
const List<ModuleGroup>& ModuleGroups::groups() const
{
	return groups_;
}

// Return reflist of all Modules present over all groups
const RefDataList<Module,ModuleGroup*>& ModuleGroups::modules() const
{
	return allModules_;
}

// Return whether the specified Module is present (in any group)
bool ModuleGroups::contains(Module* module) const
{
	return allModules_.contains(module);
}

// Return name of group assigned to specified Module (if present)
const char* ModuleGroups::groupName(Module* module) const
{
	RefDataItem<Module,ModuleGroup*>* ri = allModules_.contains(module);

	return (ri ? ri->data()->name() : "Default");
}
