// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/group.h"
#include <memory>
#include <vector>

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
    std::vector<std::unique_ptr<ModuleGroup>> groups_;
    // Vector of all Modules present in all groups
    std::vector<std::pair<Module *, ModuleGroup *>> allModules_;

    public:
    // Add Module to specified group, creating it if necessary
    ModuleGroup *addModule(Module *module, std::string_view groupName);
    // Remove Module
    void removeModule(Module *module);
    // Number of Modules present of all groups
    int nModules() const;
    // Return current list of groups
    const std::vector<std::unique_ptr<ModuleGroup>> &groups() const;
    // Return reflist of all Modules present over all groups
    const std::vector<std::pair<Module *, ModuleGroup *>> &modules() const;
    // Return whether the specified Module is present (in any group)
    bool contains(const Module *module) const;
    // Return name of group assigned to specified Module (if present)
    std::string_view groupName(const Module *module) const;
};
