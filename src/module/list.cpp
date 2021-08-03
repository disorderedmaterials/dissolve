// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "module/list.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "module/module.h"

ModuleList::ModuleList() = default;

ModuleList::~ModuleList() = default;

ModuleList::operator std::vector<std::unique_ptr<Module>> &() { return modules_; }

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
        {
            auto pos = std::find_if(modules_.begin(), modules_.end(),
                                    [addBeforeThis](auto &mod) { return mod.get() == addBeforeThis; });
            modules_.emplace(pos, module);
        }
    }
    else
        modules_.emplace_back(module);

    return true;
}

// Remove specified Module from list (but don't delete it)
void ModuleList::cut(Module *module)
{
    modules_.erase(std::find_if(modules_.begin(), modules_.end(), [module](auto &mod) { return mod.get() == module; }));
}

// Find associated Module by unique name
Module *ModuleList::find(std::string_view uniqueName) const
{
    for (auto &module : modules_)
        if (DissolveSys::sameString(module->uniqueName(), uniqueName))
            return module.get();

    return nullptr;
}

// Return whether specified Module is present in the list
bool ModuleList::contains(Module *searchModule) const
{
    for (auto &module : modules_)
        if (module.get() == searchModule)
            return true;

    return false;
}

// Return total number of Modules in the list
int ModuleList::nModules() const { return modules_.size(); }

// Return list of Modules
std::vector<std::unique_ptr<Module>> &ModuleList::modules() { return modules_; }

/*
 * General Actions
 */

// Run set-up stages for all modules
bool ModuleList::setUpAll(Dissolve &dissolve, ProcessPool &procPool)
{
    for (auto &module : modules_)
        if (!module->setUp(dissolve, procPool))
            return false;

    return true;
}
