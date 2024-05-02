// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "module/groups.h"
#include "base/sysFunc.h"
#include "module/group.h"
#include <algorithm>

/*
 * Module Types
 */

// Add allowed Module type
void ModuleGroups::addAllowedModuleType(std::string_view moduleType)
{
    // Check if the provided type is already present...
    if (moduleTypeIsAllowed(moduleType))
        return;

    allowedModuleTypes_.emplace_back(moduleType);
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
    auto moduleGroup =
        std::find_if(groups_.begin(), groups_.end(),
                     [groupName](auto &moduleGroup) { return DissolveSys::sameString(moduleGroup->name(), groupName); });

    if (moduleGroup == groups_.end())
    {
        groups_.emplace_back(std::make_unique<ModuleGroup>(groupName));
        moduleGroup = groups_.end() - 1;
    }

    // Is the Module already in the list and assigned to a group?
    auto moduleItem =
        std::find_if(allModules_.begin(), allModules_.end(), [module](const auto &pair) { return pair.first == module; });
    if (moduleItem != allModules_.end())
        moduleItem->second->remove(module);
    else
        allModules_.emplace_back(module, moduleGroup->get());

    // Add the Module to its new group
    moduleGroup->get()->add(module);

    return moduleGroup->get();
}

// Remove Module
void ModuleGroups::removeModule(Module *module)
{
    // Find the Module in our list
    auto moduleItem =
        std::find_if(allModules_.begin(), allModules_.end(), [module](const auto &pair) { return pair.first == module; });
    if (moduleItem == allModules_.end())
        return;

    auto *group = moduleItem->second;

    group->remove(module);
    allModules_.erase(moduleItem);

    // Is the group now empty?
    if (group->nModules() == 0)
        groups_.erase(std::find_if(groups_.begin(), groups_.end(), [group](auto &g) { return g.get() == group; }));
}

// Number of Modules present of all groups
int ModuleGroups::nModules() const { return allModules_.size(); }

// Return current list of groups
const std::vector<std::unique_ptr<ModuleGroup>> &ModuleGroups::groups() const { return groups_; }

// Return reflist of all Modules present over all groups
const std::vector<std::pair<Module *, ModuleGroup *>> &ModuleGroups::modules() const { return allModules_; }

// Return whether the specified Module is present (in any group)
bool ModuleGroups::contains(const Module *module) const
{
    return allModules_.end() !=
           std::find_if(allModules_.begin(), allModules_.end(), [module](const auto &pair) { return pair.first == module; });
}

// Return name of group assigned to specified Module (if present)
std::string_view ModuleGroups::groupName(const Module *module) const
{
    auto ri = std::find_if(allModules_.begin(), allModules_.end(), [module](const auto &pair) { return pair.first == module; });

    return (ri != allModules_.end() ? ri->second->name() : "Default");
}
