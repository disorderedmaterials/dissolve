// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "module/group.h"
#include "base/sysfunc.h"

ModuleGroup::ModuleGroup(std::string_view name) : ListItem<ModuleGroup>(), name_{name} {}

/*
 * Module Group
 */

// Return name of group
std::string_view ModuleGroup::name() const { return name_; }

// Return whether name of group is as specified
bool ModuleGroup::isName(std::string_view queryName) const { return DissolveSys::sameString(name_, queryName); }

// Associate Module to group
void ModuleGroup::add(Module *module) { modules_.addUnique(module); }

// Remove Module from group
void ModuleGroup::remove(Module *module) { modules_.remove(module); }

// Return whether the specified Module is in the group
bool ModuleGroup::contains(Module *module) const { return modules_.contains(module); }

// Return total number of Modules in the group
int ModuleGroup::nModules() const { return modules_.nItems(); }

// Return reflist of Modules
const RefList<Module> &ModuleGroup::modules() const { return modules_; }
