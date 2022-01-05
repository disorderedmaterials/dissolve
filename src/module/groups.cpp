// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
    auto moduleGroup = std::find_if(groups_.begin(), groups_.end(), [groupName](auto &moduleGroup) {
        return DissolveSys::sameString(moduleGroup->name(), groupName);
    });

    if (moduleGroup == groups_.end())
    {
        groups_.emplace_back(std::make_unique<ModuleGroup>(groupName));
        moduleGroup = groups_.end() - 1;
    }

    // Is the Module already in the list and assigned to a group?
    RefDataItem<Module, ModuleGroup *> *moduleItem = allModules_.contains(module);
    if (moduleItem)
        moduleItem->data()->remove(module);
    else
        allModules_.append(module, moduleGroup->get());

    // Add the Module to its new group
    moduleGroup->get()->add(module);

    return moduleGroup->get();
}

// Remove Module
void ModuleGroups::removeModule(Module *module)
{
    // Find the Module in our list
    RefDataItem<Module, ModuleGroup *> *moduleItem = allModules_.contains(module);
    if (!moduleItem)
        return;

    auto *group = moduleItem->data();

    group->remove(module);
    allModules_.remove(module);

    // Is the group now empty?
    if (group->nModules() == 0)
        groups_.erase(std::find_if(groups_.begin(), groups_.end(), [group](auto &g) { return g.get() == group; }));
}

// Number of Modules present of all groups
int ModuleGroups::nModules() const { return allModules_.nItems(); }

// Return current list of groups
const std::vector<std::unique_ptr<ModuleGroup>> &ModuleGroups::groups() const { return groups_; }

// Return reflist of all Modules present over all groups
const RefDataList<Module, ModuleGroup *> &ModuleGroups::modules() const { return allModules_; }

// Return whether the specified Module is present (in any group)
bool ModuleGroups::contains(const Module *module) const { return allModules_.contains(module); }

// Return name of group assigned to specified Module (if present)
std::string_view ModuleGroups::groupName(const Module *module) const
{
    auto *ri = allModules_.contains(module);

    return (ri ? ri->data()->name() : "Default");
}
