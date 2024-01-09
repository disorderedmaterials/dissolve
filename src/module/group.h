// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <string>
#include <string_view>
#include <vector>

// Forward Declarations
class Module;

// ModuleGroup
class ModuleGroup
{
    public:
    ModuleGroup(std::string_view name = "");
    ~ModuleGroup() = default;

    /*
     * Module Group
     */
    private:
    // Name of the group
    std::string name_;
    // vector of Modules in this group
    std::vector<Module *> modules_;

    public:
    // Return name of group
    std::string_view name() const;
    // Return whether name of group is as specified
    bool isName(std::string_view queryName) const;
    // Associate Module to group
    void add(Module *module);
    // Remove Module from group
    void remove(Module *module);
    // Return whether the specified Module is in the group
    bool contains(Module *module) const;
    // Return number of Modules in the group
    int nModules() const;
    // Return reflist of Modules
    const std::vector<Module *> &modules() const;
};
