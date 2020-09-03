/*
    *** Module Group
    *** src/module/group.cpp
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
