/*
    *** Module List
    *** src/module/list.h
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

#include "templates/list.h"

// Forward Declarations
class Dissolve;
class Module;
class GenericList;
class ProcessPool;

// ModuleList
class ModuleList
{
    public:
    ModuleList();
    ~ModuleList();
    operator List<Module> &();

    /*
     * Module List
     */
    private:
    // List of Modules
    List<Module> modules_;

    public:
    // Clear list
    void clear();
    // Associate Module to the list
    bool own(Module *module, Module *addBeforeThis = nullptr);
    // Remove specified Module from list (but don't delete it)
    void cut(Module *module);
    // Find associated Module by unique name
    Module *find(std::string_view uniqueName) const;
    // Return whether specified Module is present in the list
    bool contains(Module *searchModule) const;
    // Return number of Modules in the list
    int nModules() const;
    // Return list of Modules
    List<Module> &modules();

    /*
     * General Actions
     */
    public:
    // Run set-up stages for all modules
    bool setUpAll(Dissolve &dissolve, ProcessPool &procPool);
};
