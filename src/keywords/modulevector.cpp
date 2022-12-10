// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/modulevector.h"
#include "base/lineparser.h"
#include "classes/coredata.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <utility>

ModuleVectorKeyword::ModuleVectorKeyword(std::vector<Module *> &data, std::optional<int> maxModules)
    : KeywordBase(typeid(this)), data_(data), maxModules_(maxModules)
{
}

ModuleVectorKeyword::ModuleVectorKeyword(std::vector<Module *> &data, std::vector<std::string> allowedModuleTypes,
                                         std::optional<int> maxModules)
    : KeywordBase(typeid(this)), data_(data), moduleTypes_(std::move(allowedModuleTypes)), maxModules_(maxModules)
{
}

/*
 * Data
 */

// Return the data vector
std::vector<Module *> &ModuleVectorKeyword::data() { return data_; }
const std::vector<Module *> &ModuleVectorKeyword::data() const { return data_; }

// Return the Module type(s) to allow
const std::vector<std::string> &ModuleVectorKeyword::moduleTypes() const { return moduleTypes_; }

// Return maximum number of Modules to allow
std::optional<int> ModuleVectorKeyword::maxModules() const { return maxModules_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> ModuleVectorKeyword::maxArguments() const { return maxModules(); }

// Deserialise from supplied LineParser, starting at given argument offset
bool ModuleVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments provided to the keyword
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Find specified Module by its unique name
        Module *module = Module::find(parser.argsv(n));
        if (!module)
            return Messenger::error("No Module named '{}' exists.\n", parser.argsv(n));

        // Check the module's type if we can
        if (!moduleTypes_.empty() &&
            std::find_if(moduleTypes_.cbegin(), moduleTypes_.cend(), [module](const auto &s) { return s == module->type(); }) ==
                moduleTypes_.cend())
            return Messenger::error("Module '{}' is of type '{}', and is not relevant to keyword '{}' (allowed types = {}).\n",
                                    parser.argsv(n), module->type(), name(), joinStrings(moduleTypes_));

        data_.emplace_back(module);
    }

    return true;
}

// Serialise data to specified LineParser
bool ModuleVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto *module : data_)
        if (!parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, module->name()))
            return false;

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
