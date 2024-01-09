// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <any>
#include <functional>
#include <string>
#include <typeindex>
#include <unordered_map>

// GenericItem Producer
class GenericItemProducer
{
    private:
    GenericItemProducer();

    public:
    GenericItemProducer(const GenericItemProducer &) = delete;
    GenericItemProducer(GenericItemProducer &&) = delete;
    GenericItemProducer &operator=(const GenericItemProducer &) = delete;
    GenericItemProducer &operator=(GenericItemProducer &&) = delete;

    /*
     * Producers
     */
    private:
    // Producer function type
    using ProducerFunction = std::function<std::any()>;
    // Producers for all data types
    std::unordered_map<std::type_index, std::string> classNames_;
    std::unordered_map<std::type_index, ProducerFunction> producers_;

    private:
    // Register producer for specific class
    template <class T> void registerProducer(std::string className)
    {
        producers_[typeid(T)] = []() { return T(); };
        classNames_[typeid(T)] = std::move(className);
    }
    // Produce object of specified type
    std::any produce(const std::type_info &objectType) const;
    // Produce object of named class
    std::any produce(const std::string_view className) const;
    // Return class name for specified type
    std::string className(const std::type_info &objectType) const;

    /*
     * Instance
     */
    private:
    // Return the producer instance
    static const GenericItemProducer &instance();

    /*
     * Static Functions
     */
    public:
    // Create new item via template
    template <class T> static std::any create() { return instance().produce(typeid(T)); }
    // Create new item of the named class type
    static std::any create(std::string_view className);
    // Return registered class name for template type
    template <class T> static std::string className() { return instance().className(typeid(T)); }
};
