// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/coredata.h"
#include <any>
#include <map>
#include <typeindex>

// GenericItem Serialiser
class GenericItemSerialiser
{
    private:
    GenericItemSerialiser();

    public:
    GenericItemSerialiser(const GenericItemSerialiser &) = delete;
    GenericItemSerialiser &operator=(const GenericItemSerialiser &) = delete;

    /*
     * Serialisers
     */
    private:
    // Serialisation function type
    using SerialiseFunction = std::function<bool(const std::any &a, LineParser &parser, const CoreData &coreData)>;
    // Serialisers for all data types
    std::unordered_map<std::type_index, SerialiseFunction> serialisers_;

    private:
    // Register serialiser for specific class
    template <class T> void registerSerialiser(SerialiseFunction func) { serialisers_[typeid(T)] = std::move(func); }
    // Serialise object of specified type
    bool serialiseObject(const std::any &a, LineParser &parser, const CoreData &coreData) const;
    // Serialise templated object
    template <class T> bool serialiseObject(const T &object, LineParser &parser, const CoreData &coreData) const
    {
        // Find a suitable serialiser and call it
        auto it = serialisers_.find(typeid(T));
        if (it == serialisers_.end())
            throw(std::runtime_error(fmt::format(
                "Item of type '{}' cannot be serialised as no suitable serialiser has been registered.\n", typeid(T).name())));

        return (it->second)(object, parser, coreData);
    }

    /*
     * Instance
     */
    private:
    // Return the serialiser instance
    static const GenericItemSerialiser &instance();

    /*
     * Static Functions
     */
    public:
    // Serialise templated object
    template <class T> static bool serialise(const T &object, LineParser &parser)
    {
        static CoreData dummyCoreData;
        return instance().serialiseObject<T>(object, parser, dummyCoreData);
    }
    template <class T> static bool serialise(const T &object, LineParser &parser, const CoreData &coreData)
    {
        return instance().serialiseObject<T>(object, parser, coreData);
    }
    // Serialise supplied object
    static bool serialise(const std::any &a, LineParser &parser);
    static bool serialise(const std::any &a, LineParser &parser, const CoreData &coreData);
};
