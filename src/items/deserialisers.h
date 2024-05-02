// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/lineParser.h"
#include "classes/coreData.h"
#include <any>
#include <fmt/format.h>
#include <functional>
#include <typeindex>
#include <unordered_map>

// GenericItem Deserialiser
class GenericItemDeserialiser
{
    private:
    GenericItemDeserialiser();

    public:
    GenericItemDeserialiser(const GenericItemDeserialiser &) = delete;
    GenericItemDeserialiser(GenericItemDeserialiser &&) = delete;
    GenericItemDeserialiser &operator=(const GenericItemDeserialiser &) = delete;
    GenericItemDeserialiser &operator=(GenericItemDeserialiser &&) = delete;

    /*
     * Deserialisers
     */
    private:
    // Deserialisation function type
    using DeserialiseFunction = std::function<bool(std::any &a, LineParser &parser, const CoreData &coreData)>;
    // Deserialisers for all data types
    std::unordered_map<std::type_index, DeserialiseFunction> deserialisers_, legacyDeserialisers_;

    private:
    template <class T> static bool simpleDeserialise(std::any &a, LineParser &parser, const CoreData &coreData)
    {
        return std::any_cast<T &>(a).deserialise(parser);
    }
    template <class T> static bool simpleDeserialiseCore(std::any &a, LineParser &parser, const CoreData &coreData)
    {
        return std::any_cast<T &>(a).deserialise(parser, coreData);
    }
    template <class T> static bool vectorDeserialise(std::any &a, LineParser &parser, const CoreData &coreData)
    {
        auto &v = std::any_cast<std::vector<T> &>(a);
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto nItems = parser.argi(0);
        v.resize(nItems);
        for (auto &i : v)
            if (!i.deserialise(parser))
                return false;
        return true;
    }
    // Register deserialiser for specific class
    template <class T> void registerDeserialiser(DeserialiseFunction func) { deserialisers_[typeid(T)] = std::move(func); }
    // Register legacy deserialiser for specific class
    template <class T> void registerLegacyDeserialiser(DeserialiseFunction func)
    {
        legacyDeserialisers_[typeid(T)] = std::move(func);
    }
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

        auto a = std::make_any<T>(object);
        if (!(it->second)(a, parser, coreData))
            return false;
        object = std::move(std::any_cast<T>(a));

        return true;
    }
    // Return whether the supplied object has a legacy deserialiser
    bool hasLegacyDeserialiser(std::any &object) const
    {
        return legacyDeserialisers_.find(object.type()) != legacyDeserialisers_.end();
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
    // Return whether supplied object is a legacy object
    static bool isLegacyObject(std::any &object);
};
