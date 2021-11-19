// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/registry.h"

// Create a Module instance for the named Module type
std::unique_ptr<Module> &Dissolve::createModuleInstance(std::string_view moduleType)
{
    // Find a suitable unique name for the Module
    auto instanceId = 1;
    std::string uniqueName;
    do
    {
        uniqueName = fmt::format("{}{:02d}", moduleType, instanceId++);
    } while (findModuleInstance(uniqueName));

    // Create a new instance of the specified Module and add it to our list
    auto &instance = ModuleRegistry::create(moduleType);
    moduleInstances_.append(instance.get());
    instance->setUniqueName(uniqueName);

    return instance;
}

// Create a Module instance for the named Module type, and add it to the specified layer
Module *Dissolve::createModuleInstance(std::string_view moduleType, ModuleLayer *destinationLayer)
{
    auto &instance = createModuleInstance(moduleType);
    if (!module)
        return nullptr;

    // Add the new module instance to the specified destination layer
    destinationLayer->insert(instance);

    return instance.get();
}

// Search for any instance of any Module with the specified unique name
Module *Dissolve::findModuleInstance(std::string_view uniqueName)
{
    for (auto *module : moduleInstances_)
        if (DissolveSys::sameString(module->uniqueName(), uniqueName))
            return module;

    return nullptr;
}

// Search for any instance of any Module with the specified Module type
std::vector<Module *> Dissolve::findModuleInstances(std::string_view moduleType)
{
    std::vector<Module *> instances;

    for (auto *module : moduleInstances_)
        if (DissolveSys::sameString(module->type(), moduleType))
            instances.emplace_back(module);

    return instances;
}

// Generate unique Module name with base name provided
std::string Dissolve::uniqueModuleName(std::string_view name, Module *excludeThis)
{
    std::string_view baseName = name.empty() ? "Unnamed" : name;
    std::string uniqueName{baseName};

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (const auto *existingModule = findModuleInstance(uniqueName))
    {
        if (existingModule == excludeThis)
            break;

        // Increase suffix value and regenerate uniqueName from baseName
        ++suffix;
        uniqueName = fmt::format("{}{:02d}", baseName, suffix);
    }

    return uniqueName;
}

// Delete specified Module instance
bool Dissolve::deleteModuleInstance(std::unique_ptr<Module> instance)
{
    if (!moduleInstances_.contains(instance.get()))
        return Messenger::error("Can't find Module instance to remove.\n");

    // Remove the reference from our list
    moduleInstances_.remove(instance.get());

    // Invalidate any references to the module in keywords
    KeywordStore::objectNoLongerValid(instance.get());

    return true;
}
