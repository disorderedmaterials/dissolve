// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "module/group.h"
#include "base/sysFunc.h"
#include <algorithm>

ModuleGroup::ModuleGroup(std::string_view name) : name_{name} {}

/*
 * Module Group
 */

// Return name of group
std::string_view ModuleGroup::name() const { return name_; }

// Return whether name of group is as specified
bool ModuleGroup::isName(std::string_view queryName) const { return DissolveSys::sameString(name_, queryName); }

// Associate Module to group
void ModuleGroup::add(Module *module)
{
    if (!contains(module))
        modules_.push_back(module);
}

// Remove Module from group
void ModuleGroup::remove(Module *module)
{
    modules_.erase(std::remove(modules_.begin(), modules_.end(), module), modules_.end());
}

// Return whether the specified Module is in the group
bool ModuleGroup::contains(Module *module) const
{
    auto it = std::find(modules_.begin(), modules_.end(), module);
    return it != modules_.end();
}

// Return total number of Modules in the group
int ModuleGroup::nModules() const { return modules_.size(); }

// Return reflist of Modules
const std::vector<Module *> &ModuleGroup::modules() const { return modules_; }
