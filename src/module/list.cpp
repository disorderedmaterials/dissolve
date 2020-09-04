/*
    *** Module List
    *** src/module/list.cpp
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

#include "module/list.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "module/module.h"

ModuleList::ModuleList() {}

ModuleList::~ModuleList() {}

ModuleList::operator List<Module> &() { return modules_; }

/*
 * Module List
 */

// Clear list
void ModuleList::clear() { modules_.clear(); }

// Associate module to list
bool ModuleList::own(Module *module, Module *addBeforeThis)
{
    // Add the module pointer to the list
    if (addBeforeThis)
    {
        // Find the specified Module in the list
        if (!contains(addBeforeThis))
        {
            Messenger::error("ModuleList doesn't contain the Module pointer given as 'addBeforeThis'.\n");
            return false;
        }
        else
            modules_.ownBefore(module, addBeforeThis);
    }
    else
        modules_.own(module);

    return true;
}

// Remove specified Module from list (but don't delete it)
void ModuleList::cut(Module *module) { modules_.cut(module); }

// Find associated Module by unique name
Module *ModuleList::find(std::string_view uniqueName) const
{
    ListIterator<Module> moduleIterator(modules_);
    while (Module *module = moduleIterator.iterate())
        if (DissolveSys::sameString(module->uniqueName(), uniqueName))
            return module;

    return nullptr;
}

// Return whether specified Module is present in the list
bool ModuleList::contains(Module *searchModule) const
{
    ListIterator<Module> moduleIterator(modules_);
    while (Module *module = moduleIterator.iterate())
        if (module == searchModule)
            return true;

    return false;
}

// Return total number of Modules in the list
int ModuleList::nModules() const { return modules_.nItems(); }

// Return list of Modules
List<Module> &ModuleList::modules() { return modules_; }

/*
 * General Actions
 */

// Run set-up stages for all modules
bool ModuleList::setUpAll(Dissolve &dissolve, ProcessPool &procPool)
{
    ListIterator<Module> moduleIterator(modules_);
    while (Module *module = moduleIterator.iterate())
        if (!module->setUp(dissolve, procPool))
            return false;

    return true;
}
