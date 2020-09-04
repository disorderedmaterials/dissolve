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
#include "base/sysfunc.h"
#include "module/group.h"

/*
 * Module Types
 */

// Add allowed Module type
void ModuleGroups::addAllowedModuleType(std::string_view moduleType)
{
    // Check if the provided type is already present...
    if (moduleTypeIsAllowed(moduleType))
        return;

    allowedModuleTypes_.push_back(std::string(moduleType));
}

// Set list of allowed Module types
void ModuleGroups::setAllowedModuleTypes(const std::vector<std::string> &moduleTypes) { allowedModuleTypes_ = moduleTypes; }

// Return if specified Module type is allowed in any group
bool ModuleGroups::moduleTypeIsAllowed(std::string_view moduleType) const
{
    return std::find_if(allowedModuleTypes_.cbegin(), allowedModuleTypes_.cend(),
                        [moduleType](const auto &s) { return s == moduleType; }) != allowedModuleTypes_.cend();
}

// Return list of allowed Module types
const std::vector<std::string> &ModuleGroups::allowedModuleTypes() const { return allowedModuleTypes_; }

/*
 * Module Groups
 */

// Add Module to specified group, creating it if necessary
ModuleGroup *ModuleGroups::addModule(Module *module, std::string_view groupName)
{
    // Does the specified group exist?
    ModuleGroup *moduleGroup;
    for (moduleGroup = groups_.first(); moduleGroup != nullptr; moduleGroup = moduleGroup->next())
        if (DissolveSys::sameString(moduleGroup->name(), groupName))
            break;

    if (moduleGroup == nullptr)
    {
        moduleGroup = new ModuleGroup(groupName);
        groups_.own(moduleGroup);
    }

    // Is the Module already in the list and assigned to a group?
    RefDataItem<Module, ModuleGroup *> *moduleItem = allModules_.contains(module);
    if (moduleItem)
        moduleItem->data()->remove(module);
    else
        allModules_.append(module, moduleGroup);

    // Add the Module to its new group
    moduleGroup->add(module);

    return moduleGroup;
}

// Remove Module
void ModuleGroups::removeModule(Module *module)
{
    // Find the Module in our list
    RefDataItem<Module, ModuleGroup *> *moduleItem = allModules_.contains(module);
    if (!moduleItem)
        return;

    ModuleGroup *group = moduleItem->data();

    group->remove(module);
    allModules_.remove(module);

    // Is the group now empty?
    if (group->nModules() == 0)
        groups_.remove(group);
}

// Number of Modules present of all groups
int ModuleGroups::nModules() const { return allModules_.nItems(); }

// Return current list of groups
const List<ModuleGroup> &ModuleGroups::groups() const { return groups_; }

// Return reflist of all Modules present over all groups
const RefDataList<Module, ModuleGroup *> &ModuleGroups::modules() const { return allModules_; }

// Return whether the specified Module is present (in any group)
bool ModuleGroups::contains(Module *module) const { return allModules_.contains(module); }

// Return name of group assigned to specified Module (if present)
std::string_view ModuleGroups::groupName(Module *module) const
{
    auto *ri = allModules_.contains(module);

    return (ri ? ri->data()->name() : "Default");
}
