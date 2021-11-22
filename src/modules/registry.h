// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/module.h"
#include <map>

// Forward Declarations
class KeywordWidgetBase;
class QWidget;

// Module Producer
class ModuleRegistry
{
    private:
    ModuleRegistry();

    public:
    ModuleRegistry(const ModuleRegistry &) = delete;
    ModuleRegistry(ModuleRegistry &&) = delete;
    ModuleRegistry &operator=(const ModuleRegistry &) = delete;
    ModuleRegistry &operator=(ModuleRegistry &&) = delete;

    /*
     * Producers
     */
    private:
    // Producer function type
    using ProducerFunction = std::function<Module *()>;
    // Typedefs
    using ModuleRegistryData = std::pair<ProducerFunction, std::string>;
    using ModuleInfoData = std::pair<std::string, std::string>;
    // Producers for all module types
    std::map<std::string, ModuleRegistryData> producers_;
    // Categorised map of modules
    std::vector<std::pair<std::string, std::vector<ModuleRegistry::ModuleInfoData>>> categories_;

    private:
    // Register producer for module
    template <class M> void registerProducer(std::string moduleType, std::string brief, std::string category = "")
    {
        // Check for duplicate module type
        if (producers_.find(moduleType) != producers_.end())
            throw(std::runtime_error(fmt::format("A module producer for type '{}' already exists.\n", moduleType)));

        producers_.emplace(moduleType, ModuleRegistryData([]() { return new M(); }, brief));
        if (!category.empty())
        {
            auto it =
                std::find_if(categories_.begin(), categories_.end(), [category](const auto &c) { return c.first == category; });
            if (it != categories_.end())
                it->second.emplace_back(ModuleInfoData(moduleType, brief));
            else
                categories_.emplace_back(category, std::vector<ModuleInfoData>{{moduleType, brief}});
        }
    }
    // Produce module of specified type
    Module *produce(std::string moduleType) const;
    // Return categorised map of modules
    const std::vector<std::pair<std::string, std::vector<ModuleRegistry::ModuleInfoData>>> &categories() const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const ModuleRegistry &instance();

    /*
     * Module Management
     */
    public:
    // Return category map
    static const std::vector<std::pair<std::string, std::vector<ModuleRegistry::ModuleInfoData>>> &categoryMap();
    // Create new item via template
    static std::unique_ptr<Module> create(std::string_view moduleType);
    // Create a Module instance for the named Module type, and add it to the specified layer
    static Module *create(std::string_view moduleType, ModuleLayer *destinationLayer);
};
