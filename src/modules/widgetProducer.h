// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <cassert>
#include <functional>
#include <typeindex>
#include <unordered_map>

// Forward Declarations
class Dissolve;
class Module;
class ModuleWidget;

// ModuleWidget Producer
class ModuleWidgetProducer
{
    private:
    ModuleWidgetProducer();

    public:
    ModuleWidgetProducer(const ModuleWidgetProducer &) = delete;
    ModuleWidgetProducer(ModuleWidgetProducer &&) = delete;
    ModuleWidgetProducer &operator=(const ModuleWidgetProducer &) = delete;
    ModuleWidgetProducer &operator=(ModuleWidgetProducer &&) = delete;

    /*
     * Producers
     */
    private:
    // Producer function type
    using ProducerFunction = std::function<ModuleWidget *(Module *module, Dissolve &dissolve)>;
    // Producers for all data types
    std::unordered_map<std::type_index, ProducerFunction> producers_;

    private:
    // Register producer for specific class
    template <class M, class W> void registerProducer()
    {
        producers_[typeid(M)] = [](Module *module, Dissolve &dissolve)
        {
            auto *derived = dynamic_cast<M *>(module);
            assert(derived);
            return new W(nullptr, derived, dissolve);
        };
    }
    // Produce object of specified type
    ModuleWidget *produce(Module *module, Dissolve &dissolve) const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const ModuleWidgetProducer &instance();

    /*
     * Static Functions
     */
    public:
    // Create new item via template
    static ModuleWidget *create(Module *module, Dissolve &dissolve) { return instance().produce(module, dissolve); }
};
