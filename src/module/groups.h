/*
    *** Module Groups
    *** src/module/groups.h
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

#pragma once

#include "module/group.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Module;
class ModuleGroup;

// Set of Module Groups
class ModuleGroups
{
    public:
    ModuleGroups() = default;
    ~ModuleGroups() = default;

    /*
     * Module Types
     */
    private:
    // List of allowed Module types in any group
    std::vector<std::string> allowedModuleTypes_;

    public:
    // Add allowed Module type
    void addAllowedModuleType(std::string_view moduleType);
    // Set list of allowed Module types
    void setAllowedModuleTypes(const std::vector<std::string> &moduleTypes);
    // Return if specified Module type is allowed in any group
    bool moduleTypeIsAllowed(std::string_view moduleType) const;
    // Return list of allowed Module types
    const std::vector<std::string> &allowedModuleTypes() const;

    /*
     * Module Groups
     */
    private:
    // Current list of groups
    List<ModuleGroup> groups_;
    // RefList of all Modules present in all groups
    RefDataList<Module, ModuleGroup *> allModules_;

    public:
    // Add Module to specified group, creating it if necessary
    ModuleGroup *addModule(Module *module, std::string_view groupName);
    // Remove Module
    void removeModule(Module *module);
    // Number of Modules present of all groups
    int nModules() const;
    // Return current list of groups
    const List<ModuleGroup> &groups() const;
    // Return reflist of all Modules present over all groups
    const RefDataList<Module, ModuleGroup *> &modules() const;
    // Return whether the specified Module is present (in any group)
    bool contains(Module *module) const;
    // Return name of group assigned to specified Module (if present)
    std::string_view groupName(Module *module) const;
};
