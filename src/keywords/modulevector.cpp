// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/modulevector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "module/list.h"
#include "module/module.h"

ModuleVectorKeyword::ModuleVectorKeyword(int maxModules)
    : KeywordData<std::vector<Module *>>(KeywordBase::ModuleRefListData, {}), maxModules_(maxModules)
{
}

ModuleVectorKeyword::ModuleVectorKeyword(const std::vector<std::string> &allowedModuleTypes, int maxModules)
    : KeywordData<std::vector<Module *>>(KeywordBase::ModuleRefListData, {}), moduleTypes_(allowedModuleTypes),
      maxModules_(maxModules)
{
}

ModuleVectorKeyword::~ModuleVectorKeyword() {}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ModuleVectorKeyword::isDataEmpty() const { return data_.empty(); }

// Return the Module type(s) to allow
const std::vector<std::string> &ModuleVectorKeyword::moduleTypes() const { return moduleTypes_; }

// Return maximum number of Modules to allow in the list
int ModuleVectorKeyword::maxModules() const { return maxModules_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ModuleVectorKeyword::minArguments() const { return 1; }

// Return maximum number of arguments accepted
int ModuleVectorKeyword::maxArguments() const { return (maxModules_ == -1 ? 99 : maxModules_); }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ModuleVectorKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments provided to the keyword
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Find specified Module by its unique name
        Module *module = coreData.findModule(parser.argsv(n));
        if (!module)
            return Messenger::error("No Module named '{}' exists.\n", parser.argsv(n));

        // Check the module's type (if a list has been specified)
        if (!moduleTypes_.empty() && std::find_if(moduleTypes_.cbegin(), moduleTypes_.cend(), [module](const auto &s) {
                                         return s == module->type();
                                     }) == moduleTypes_.cend())
        {
            std::string allowedTypes;
            for (const auto &s : moduleTypes_)
                allowedTypes += allowedTypes.empty() ? s : ", " + s;

            return Messenger::error("Module '{}' is of type '{}', and is not permitted in this list (allowed types = {}).\n",
                                    parser.argsv(n), module->type(), allowedTypes);
        }

        data_.emplace_back(module);
    }

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ModuleVectorKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over list of referenced Modules
    for (auto *module : data_)
    {
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, module->uniqueName()))
            return false;
    }

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Module in the contained data
void ModuleVectorKeyword::removeReferencesTo(Module *module)
{
    auto it = std::find(data_.begin(), data_.end(), module);
    if (it != data_.end())
        data_.erase(it);
}
