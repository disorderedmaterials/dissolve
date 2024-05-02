// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "classes/coreData.h"
#include <any>
#include <typeindex>
#include <unordered_map>

// GenericItem Serialiser
class GenericItemSerialiser
{
    private:
    GenericItemSerialiser();

    public:
    GenericItemSerialiser(const GenericItemSerialiser &) = delete;
    GenericItemSerialiser(GenericItemSerialiser &&) = delete;
    GenericItemSerialiser &operator=(const GenericItemSerialiser &) = delete;
    GenericItemSerialiser &operator=(GenericItemSerialiser &&) = delete;

    /*
     * Serialisers
     */
    private:
    // Serialisation function type
    using SerialiseFunction = std::function<bool(const std::any &a, LineParser &parser)>;
    // Serialisers for all data types
    std::unordered_map<std::type_index, SerialiseFunction> serialisers_;

    private:
    template <class T> static bool simpleSerialise(const std::any &a, LineParser &parser)
    {
        return std::any_cast<const T &>(a).serialise(parser);
    }
    template <class T> static bool vectorSerialise(const std::any &a, LineParser &parser)
    {
        const auto &v = std::any_cast<const std::vector<T> &>(a);
        if (!parser.writeLineF("{}  # size\n", v.size()))
            return false;
        for (const auto &i : v)
            if (!i.serialise(parser))
                return false;
        return true;
    }
    // Register serialiser for specific class
    template <class T> void registerSerialiser(SerialiseFunction func) { serialisers_[typeid(T)] = std::move(func); }
    // Serialise object of specified type
    bool serialiseObject(const std::any &a, LineParser &parser) const;
    // Serialise templated object
    template <class T> bool serialiseObject(const T &object, LineParser &parser) const
    {
        // Find a suitable serialiser and call it
        auto it = serialisers_.find(typeid(T));
        if (it == serialisers_.end())
            throw(std::runtime_error(fmt::format(
                "Item of type '{}' cannot be serialised as no suitable serialiser has been registered.\n", typeid(T).name())));

        return (it->second)(object, parser);
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
        return instance().serialiseObject<T>(object, parser);
    }
    // Serialise supplied object
    static bool serialise(const std::any &a, LineParser &parser);
};
