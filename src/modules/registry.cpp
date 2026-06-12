// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "modules/registry.h"

ModuleRegistry::ModuleRegistry() {}

/*
 * Producers
 */

// Produce module of specified type
Module *ModuleRegistry::produce(ModuleTypes::ModuleType moduleType) const
{
    auto it = producers_.find(moduleType);
    if (it == producers_.end())
        return {};

    return (it->second.first)();
}

// Return categorised map of modules
const std::map<std::string, std::vector<ModuleRegistry::ModuleInfoData>> &ModuleRegistry::categories() const
{
    return categories_;
}

/*
 * Instance
 */

// Return the producer instance
const ModuleRegistry &ModuleRegistry::instance()
{
    static ModuleRegistry instance;

    return instance;
}

/*
 * Module Management
 */

// Return category map
const std::map<std::string, std::vector<ModuleRegistry::ModuleInfoData>> &ModuleRegistry::categoryMap()
{
    return instance().categories();
}

// Create a Module instance for the named Module type
std::unique_ptr<Module> ModuleRegistry::create(CoreData &coreData, ModuleTypes::ModuleType type)
{
    auto m = std::unique_ptr<Module>(instance().produce(type));

    if (m)
        m->setName(DissolveSys::uniqueName(ModuleTypes::moduleType(m->type()), coreData.moduleInstances(),
                                           [&](const auto &inst) { return inst == m.get() ? "" : inst->name(); }));
    return m;
}

// Create a Module instance for the specified Module type, and add it to the specified layer
Module *ModuleRegistry::create(CoreData &coreData, ModuleTypes::ModuleType moduleType, ModuleLayer *destinationLayer)
{
    auto instance = create(coreData, moduleType);
    if (!instance)
        return nullptr;

    // Add the new module instance to the specified destination layer
    auto *m = instance.get();
    destinationLayer->modules().emplace_back(std::move(instance));

    return m;
}
