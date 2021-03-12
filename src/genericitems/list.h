// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <any>
#include <map>
#include <typeindex>

// Generic List
class GenericList
{
    public:
    GenericList();
    ~GenericList() = default;
    // Item Data Typedef
    typedef std::tuple<std::any, std::string, int, int> GenericItem;
    // Item Data Tuple Indices
    enum ItemData
    {
        AnyObject,
        ClassName,
        Version,
        Flags
    };
    // Item Flags
    enum ItemFlag
    {
        NoFlags = 0,                  /* No flags */
        InRestartFileFlag = 1,        /* The item should be written to the restart file */
        IsReferencePointDataFlag = 2, /* The item was loaded as reference point data */
        ProtectedFlag = 4             /* The item will not be clear()'d unless forced */
    };

    /*
     * Child Items
     */
    private:
    // Map of items, where the tuple corresponds to <object, className, version, flags>
    std::map<std::string, GenericItem> items_;

    public:
    // Clear all items (except those that are marked protected)
    void clear();
    // Clear all items, including protected items
    void clearAll();
    // Return whether the named item is contained in the list
    bool contains(std::string_view name, std::string_view prefix = "") const;
    // Return item list
    const std::map<std::string, GenericItem> &items() const;
    // Return the named item from the map, if it exists
    GenericItem &find(std::string_view name, std::string_view prefix = "");
    const GenericItem &find(std::string_view name, std::string_view prefix = "") const;
    // Return the version of the named item from the list
    int version(std::string_view name, std::string_view prefix = "") const;
    // Remove named item
    void remove(std::string_view name, std::string_view prefix);
    // Rename item
    void rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName, std::string_view newPrefix);
    // Prune all items with '@suffix'
    void pruneWithSuffix(std::string_view suffix);

    /*
     * Item Creation
     */
    public:
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
    // Register all producers
    void registerProducers();
    // Create new item via template
    template <class T> GenericItem &create(std::string_view name, int version, int flags)
    {
        auto it = producers_.find(typeid(T));
        if (it == producers_.end())
            throw(std::runtime_error(
                fmt::format("A producer has not been registered for type '{}', so object '{}' cannot be created.\n",
                            typeid(T).name(), name)));

        items_.emplace(name, GenericItem((it->second)(), classNames_[typeid(T)], version, flags));

        return items_[std::string(name)];
    }
    // Create new item of the named class type
    GenericItem &create(std::string_view name, std::string_view className, int version, int flags);

    public:
    // Create or retrieve named item as templated type
    template <class T> T &realise(std::string_view name, std::string_view prefix = "", int flags = -1, bool *created = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        auto it = items_.find(varName);
        if (it != items_.end())
        {
            // Check type before we attempt to cast it
            if (std::get<ItemData::AnyObject>(it->second).type() != typeid(T))
                throw(
                    std::runtime_error(fmt::format("GenericList::realise() - Item named '{}' exists, but has a different type "
                                                   "to that requested ('{}' vs '{}').\n",
                                                   prefix.empty() ? name : fmt::format("{}_{}", prefix, name),
                                                   std::get<ItemData::AnyObject>(it->second).type().name(), typeid(T).name())));

            // Bump version of the item and return it
            ++std::get<ItemData::Version>(it->second);
            if (created)
                *created = false;
            return std::any_cast<T &>(std::get<ItemData::AnyObject>(it->second));
        }

        // Create and return new item
        auto &item = create<T>(varName, 0, flags);
        if (created)
            *created = true;
        return std::any_cast<T &>(std::get<ItemData::AnyObject>(item));
    }

    /*
     * Item Retrieval
     */
    public:
    // Return named (const) item as templated type
    template <class T> const T &value(std::string_view name, std::string_view prefix = "") const
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
        if (it == items_.end())
            throw(std::runtime_error(fmt::format("GenericList::value() - Item named '{}' does not exist.\n",
                                                 prefix.empty() ? name : fmt::format("{}_{}", prefix, name))));

        // Check type before we attempt to cast it
        if (std::get<ItemData::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format(
                "GenericList::value() - Item named '{}' exists, but has a different type to that requested ('{}' vs '{}').\n",
                prefix.empty() ? name : fmt::format("{}_{}", prefix, name),
                std::get<ItemData::AnyObject>(it->second).type().name(), typeid(T).name())));

        return std::any_cast<const T &>(std::get<ItemData::AnyObject>(it->second));
    }
    // Return copy of named item as templated type, or a default value
    template <class T> T valueOr(std::string_view name, std::string_view prefix, T valueIfNotFound) const
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
        if (it == items_.end())
            return valueIfNotFound;

        // Check type before we attempt to cast it
        if (std::get<ItemData::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format(
                "GenericList::value() - Item named '{}' exists, but has a different type to that requested ('{}' vs '{}').\n",
                prefix.empty() ? name : fmt::format("{}_{}", prefix, name),
                std::get<ItemData::AnyObject>(it->second).type().name(), typeid(T).name())));

        return std::any_cast<const T>(std::get<ItemData::AnyObject>(it->second));
    }
    // Retrieve named item as templated type, assuming that it is going to be modified
    template <class T> T &retrieve(std::string_view name, std::string_view prefix = "")
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
        if (it == items_.end())
            throw(std::runtime_error(fmt::format("GenericList::retrieve() - Item named '{}' does not exist.\n",
                                                 prefix.empty() ? name : fmt::format("{}_{}", prefix, name))));

        // Check type before we attempt to cast it
        if (std::get<ItemData::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format("GenericList::retrieve() - Item named '{}' exists, but has a different type "
                                                 "to that requested ('{}' vs '{}').\n",
                                                 prefix.empty() ? name : fmt::format("{}_{}", prefix, name),
                                                 std::get<ItemData::AnyObject>(it->second).type().name(), typeid(T).name())));

        ++std::get<ItemData::Version>(it->second);
        return std::any_cast<T &>(std::get<ItemData::AnyObject>(it->second));
    }
    // Return names of all items of the template type
    template <class T> std::vector<std::string_view> all() const
    {
        std::vector<std::string_view> matches;
        for (auto &[key, value] : items_)
            if (std::get<ItemData::AnyObject>(value).type() == typeid(T))
                matches.emplace_back(key);

        return matches;
    }

    /*
     * Serialisation
     */
    private:
    // Deserialisation function type
    using DeserialiseFunction = std::function<bool(std::any &a, LineParser &parser, CoreData &coreData)>;
    // Deserialisers for all data types
    std::unordered_map<std::type_index, DeserialiseFunction> deserialisers_;
    // Serialisation function type
    using SerialiseFunction = std::function<bool(const std::any &a, LineParser &parser, const CoreData &coreData)>;
    // Serialisers for all data types
    std::unordered_map<std::type_index, SerialiseFunction> serialisers_;

    private:
    // Register deserialiser for specific class
    template <class T> void registerDeserialiser(DeserialiseFunction func) { deserialisers_[typeid(T)] = std::move(func); }
    // Register all deserialisers
    void registerDeserialisers();
    // Register serialiser for specific class
    template <class T> void registerSerialiser(SerialiseFunction func) { serialisers_[typeid(T)] = std::move(func); }
    // Register all serialisers
    void registerSerialisers();

    public:
    // Serialise all objects via the specified LineParser
    bool serialiseAll(LineParser &parser, const CoreData &coreData, std::string_view headerPrefix) const;
    // Deserialise an object from the LineParser into our map
    bool deserialise(LineParser &parser, CoreData &coreData, std::string_view name, std::string_view itemClass, int version = 0,
                     int flags = 0);

    /*
     * Parallel Comms
     */
    private:
    // Broadcast function type
    using BroadcastFunction = std::function<bool(std::any &a, ProcessPool &procPool, const int root, const CoreData &coreData)>;
    // Broadcasters for all data types
    std::unordered_map<std::type_index, BroadcastFunction> broadcasters_;

    private:
    // Register broadcaster for specific class
    template <class T> void registerBroadcaster(BroadcastFunction func) { broadcasters_[typeid(T)] = std::move(func); }
    // Register all broadcasters
    void registerBroadcasters();

    public:
    // Broadcast all data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
};
