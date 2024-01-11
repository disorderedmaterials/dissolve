// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/weightedModuleVector.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <utility>

WeightedModuleVectorKeyword::WeightedModuleVectorKeyword(std::vector<std::pair<Module *, double>> &data,
                                                         std::vector<ModuleTypes::ModuleType> allowedModuleTypes)
    : KeywordBase(typeid(this)), data_(data), moduleTypes_(std::move(allowedModuleTypes))
{
}

/*
 * Data
 */

// Return the data vector
std::vector<std::pair<Module *, double>> &WeightedModuleVectorKeyword::data() { return data_; }
const std::vector<std::pair<Module *, double>> &WeightedModuleVectorKeyword::data() const { return data_; }

// Return the Module type(s) to allow
const std::vector<ModuleTypes::ModuleType> &WeightedModuleVectorKeyword::moduleTypes() const { return moduleTypes_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> WeightedModuleVectorKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool WeightedModuleVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find specified Module by its unique name
    auto *module = Module::find(coreData, parser.argsv(startArg));
    if (!module)
        return Messenger::error("No Module named '{}' exists.\n", parser.argsv(startArg));

    // Get the weight, if specified
    auto weight = parser.hasArg(startArg + 1) ? parser.argd(startArg + 1) : 1.0;

    // Check the module's type if we can
    if (!moduleTypes_.empty() &&
        std::find_if(moduleTypes_.cbegin(), moduleTypes_.cend(),
                     [module](const auto &type) { return type == module->type(); }) == moduleTypes_.cend())
        return Messenger::error("Module '{}' is of type '{}', and is not relevant to keyword '{}' (allowed types = {}).\n",
                                module->name(), ModuleTypes::moduleType(module->type()), name(), joinStrings(moduleTypes_));

    // Check if module is already in the vector
    if (std::find_if(data_.cbegin(), data_.cend(), [module](const auto &item) { return item.first == module; }) != data_.cend())
        Messenger::warn("Module '{}' has already been added to keyword '{}'.\n", module->name(), name());
    else
        data_.emplace_back(module, weight);

    return true;
}

// Serialise data to specified LineParser
bool WeightedModuleVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto &&[module, weight] : data_)
        if (!parser.writeLineF("{}{}  '{}'  {}\n", prefix, keywordName, module->name(), weight))
            return false;

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Module in the contained data
void WeightedModuleVectorKeyword::removeReferencesTo(Module *module)
{
    auto it = std::find_if(data_.cbegin(), data_.cend(), [module](const auto &item) { return item.first == module; });
    if (it != data_.end())
        data_.erase(it);
}

// Express as a serialisable value
SerialisedValue WeightedModuleVectorKeyword::serialise() const
{
    return fromVector(data_,
                      [](const auto &item) -> SerialisedValue {
                          return {{"target", item.first->name()}, {"weight", item.second}};
                      });
}

// Read values from a serialisable value
void WeightedModuleVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this, &coreData](const auto &item)
             {
                 auto moduleName = toml::find<std::string>(item, "target");
                 auto *module = Module::find(coreData, moduleName);
                 if (!module)
                     throw toml::type_error(fmt::format("No Module named '{}' exists.\n", moduleName), item.location());

                 // Check the module's type if we can
                 if (!moduleTypes_.empty() &&
                     std::find_if(moduleTypes_.cbegin(), moduleTypes_.cend(),
                                  [module](const auto &s) { return s == module->type(); }) == moduleTypes_.cend())
                     throw toml::type_error(
                         fmt::format("Module '{}' is of type '{}', and is not relevant to keyword '{}' (allowed types = {}).\n",
                                     moduleName, module->type(), name(), joinStrings(moduleTypes_)),
                         item.location());

                 data_.emplace_back(module, toml::find<double>(item, "weight"));
             });
}

// Has not changed from initial value
bool WeightedModuleVectorKeyword::isDefault() const { return data_.empty(); }
