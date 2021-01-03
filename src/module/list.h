// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
