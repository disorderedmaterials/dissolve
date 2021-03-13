// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <any>
#include <fmt/core.h>
#include <functional>
#include <map>
#include <typeindex>

// Forward Declarations
class CoreData;
class LineParser;

// GenericItem Deserialiser
class GenericItemDeserialiser
{
    private:
    GenericItemDeserialiser();

    public:
    GenericItemDeserialiser(const GenericItemDeserialiser &) = delete;
    GenericItemDeserialiser &operator=(const GenericItemDeserialiser &) = delete;

    /*
     * Deserialisers
     */
    private:
    // Deserialisation function type
    using DeserialiseFunction = std::function<bool(std::any &a, LineParser &parser, const CoreData &coreData)>;
    // Deserialisers for all data types
    std::unordered_map<std::type_index, DeserialiseFunction> deserialisers_;

    private:
    // Register deserialiser for specific class
    template <class T> void registerDeserialiser(DeserialiseFunction func) { deserialisers_[typeid(T)] = std::move(func); }
    // Deserialise object of specified type
    bool deserialiseObject(std::any &a, LineParser &parser, const CoreData &coreData) const;
    // Deserialise templated object
    template <class T> bool deserialiseObject(T &object, LineParser &parser, const CoreData &coreData) const
    {
        // Find a suitable deserialiser and call it
        auto it = deserialisers_.find(typeid(T));
        if (it == deserialisers_.end())
            throw(std::runtime_error(
                fmt::format("Item of type '{}' cannot be deserialised as no suitable deserialiser has been registered.\n",
                            typeid(T).name())));

        auto a = std::make_any<T &>(object);
        if (!(it->second)(a, parser, coreData))
            return false;

        return true;
    }

    /*
     * Instance
     */
    private:
    // Return the deserialiser instance
    static const GenericItemDeserialiser &instance();

    /*
     * Static Functions
     */
    public:
    // Deserialise templated object
    template <class T> static bool deserialise(T &object, LineParser &parser)
    {
        static CoreData dummyCoreData;
        return instance().deserialiseObject<T>(object, parser, dummyCoreData);
    }
    template <class T> static bool deserialise(T &object, LineParser &parser, const CoreData &coreData)
    {
        return instance().deserialiseObject<T>(object, parser, coreData);
    }
    // Deserialise supplied object
    static bool deserialise(std::any &a, LineParser &parser, const CoreData &coreData);
};
