// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "items/item.h"
#include "items/producers.h"
#include "items/searchers.h"
#include "templates/optionalRef.h"
#include <any>
#include <map>
#include <typeindex>

// Generic List
class GenericList
{
    public:
    GenericList() = default;
    ~GenericList() = default;

    /*
     * Child Items
     */
    private:
    // Map of items, where the tuple corresponds to <object, className, version, flags>
    std::map<std::string, GenericItem::Type> items_;

    public:
    // Clear all items (except those that are marked protected)
    void clear();
    // Clear all items, including protected items
    void clearAll();
    // Return whether the named item is contained in the list
    bool contains(std::string_view name, std::string_view prefix = "") const;
    // Return item list
    const std::map<std::string, GenericItem::Type> &items() const;
    // Return the version of the named item from the list
    int version(std::string_view name, std::string_view prefix = "") const;
    // Remove named item
    void remove(std::string_view name, std::string_view prefix);
    // Remove all items with specified prefix
    void removeWithPrefix(std::string_view prefix);
    // Rename item
    void rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName, std::string_view newPrefix);
    // Rename prefix of items
    void renamePrefix(std::string_view oldPrefix, std::string_view newPrefix);
    // Prune all items with '@suffix'
    void pruneWithSuffix(std::string_view suffix);

    /*
     * Item Creation
     */
    public:
    // Create or retrieve named item as templated type
    template <class T> T &realise(std::string_view name, std::string_view prefix = "", int flags = GenericItem::NoFlags)
    {
        return realiseIf<T>(name, prefix, flags).first;
    }
    // Create or retrieve named item as templated type, also returning whether it was created
    template <class T>
    std::pair<T &, GenericItem::ItemStatus> realiseIf(std::string_view name, std::string_view prefix = "",
                                                      int flags = GenericItem::NoFlags)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name);

        auto it = items_.find(varName);
        if (it != items_.end())
        {
            // Check type before we attempt to cast it
            if (std::get<GenericItem::AnyObject>(it->second).type() != typeid(T))
                throw(std::runtime_error(
                    fmt::format("GenericList::realise() - Item named '{}' exists, but has a different type "
                                "to that requested ('{}' vs '{}').\n",
                                prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                                std::get<GenericItem::AnyObject>(it->second).type().name(), typeid(T).name())));

            // Bump version of the item and return it
            ++std::get<GenericItem::Version>(it->second);
            return {std::any_cast<T &>(std::get<GenericItem::AnyObject>(it->second)), GenericItem::ItemStatus::Existing};
        }

        // Create and return new item
        items_.emplace(varName,
                       GenericItem::Type(GenericItemProducer::create<T>(), GenericItemProducer::className<T>(), 0, flags));
        auto &item = items_[varName];
        return {std::any_cast<T &>(std::get<GenericItem::AnyObject>(item)), GenericItem::ItemStatus::Created};
    }

    /*
     * Item Retrieval
     */
    public:
    // Return named (const) item as templated type
    template <class T> const T &value(std::string_view name, std::string_view prefix = "") const
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
        if (it == items_.end())
            throw(std::runtime_error(fmt::format("GenericList::value() - Item named '{}' does not exist.\n",
                                                 prefix.empty() ? name : fmt::format("{}//{}", prefix, name))));

        // Check type before we attempt to cast it
        if (std::get<GenericItem::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format(
                "GenericList::value() - Item named '{}' exists, but has a different type to that requested ('{}' vs '{}').\n",
                prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                std::get<GenericItem::AnyObject>(it->second).type().name(), typeid(T).name())));

        return std::any_cast<const T &>(std::get<GenericItem::AnyObject>(it->second));
    }
    // Return copy of named item as templated type, or a default value
    template <class T> T valueOr(std::string_view name, std::string_view prefix, T valueIfNotFound) const
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
        if (it == items_.end())
            return valueIfNotFound;

        // Check type before we attempt to cast it
        if (std::get<GenericItem::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format(
                "GenericList::value() - Item named '{}' exists, but has a different type to that requested ('{}' vs '{}').\n",
                prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                std::get<GenericItem::AnyObject>(it->second).type().name(), typeid(T).name())));

        return std::any_cast<const T>(std::get<GenericItem::AnyObject>(it->second));
    }
    // Return named (const) item as templated type, if it exists
    template <class T> OptionalReferenceWrapper<const T> valueIf(std::string_view name, std::string_view prefix = "") const
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
        if (it == items_.end())
            return {};

        // Check type before we attempt to cast it
        if (std::get<GenericItem::AnyObject>(it->second).type() != typeid(T))
            throw(std::runtime_error(fmt::format(
                "GenericList::valueIf() - Item named '{}' exists, but has a different type to that requested ('{}' vs '{}').\n",
                prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                std::get<GenericItem::AnyObject>(it->second).type().name(), typeid(T).name())));

        return std::any_cast<const T &>(std::get<GenericItem::AnyObject>(it->second));
    }
    // Retrieve named item as templated type, assuming that it is going to be modified
    template <class T> T &retrieve(std::string_view name, std::string_view prefix = "")
    {
        auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
        if (it == items_.end())
            throw(std::runtime_error(fmt::format("GenericList::retrieve() - Item named '{}' does not exist.\n",
                                                 prefix.empty() ? name : fmt::format("{}//{}", prefix, name))));

        // Check type before we attempt to cast it
        if (std::get<GenericItem::AnyObject>(it->second).type() != typeid(T))
            throw(
                std::runtime_error(fmt::format("GenericList::retrieve() - Item named '{}' exists, but has a different type "
                                               "to that requested ('{}' vs '{}').\n",
                                               prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                                               std::get<GenericItem::AnyObject>(it->second).type().name(), typeid(T).name())));

        ++std::get<GenericItem::Version>(it->second);
        return std::any_cast<T &>(std::get<GenericItem::AnyObject>(it->second));
    }
    // Return names of all items of the template type
    template <class T> std::vector<std::string_view> all() const
    {
        std::vector<std::string_view> matches;
        for (auto &[key, value] : items_)
            if (std::get<GenericItem::AnyObject>(value).type() == typeid(T))
                matches.emplace_back(key);

        return matches;
    }

    /*
     * Serialisation
     */
    public:
    // Data Versions
    enum class DeserialisableDataVersion
    {
        Version08X,
        Current
    };
    // Return EnumOptions for DataVersion
    static EnumOptions<DeserialisableDataVersion> deserialisableDataVersions();

    private:
    // Current data version being deserialised
    static DeserialisableDataVersion baseDataVersion_;

    public:
    // Set current data version being deserialised by detecting it from the supplied string
    static void setBaseDataVersionFromString(std::string_view s);
    // Return current data version being deserialised
    static DeserialisableDataVersion baseDataVersion();
    // Serialise all objects via the specified LineParser
    bool serialiseAll(LineParser &parser, std::string_view headerPrefix) const;
    // Deserialise an object from the LineParser into our map
    bool deserialise(LineParser &parser, CoreData &coreData, const std::string &name, const std::string &itemClass,
                     int dataVersion = 0, int flags = 0);

    /*
     * Searchers
     */
    public:
    // Search for an object or child of the specified name
    template <class T> OptionalReferenceWrapper<const T> search(std::string_view name, std::string_view prefix = "") const
    {
        auto varName = prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name);
        auto varNamePath = varName + "//";
        for (auto &[key, value] : items_)
        {
            // Match name
            if (varName == key)
            {
                // Check type before we attempt to cast it
                if (std::get<GenericItem::AnyObject>(value).type() != typeid(T))
                    throw(std::runtime_error(fmt::format("GenericList::search() - Item named '{}' exists, but has a different "
                                                         "type to that requested ('{}' vs '{}').\n",
                                                         prefix.empty() ? name : fmt::format("{}//{}", prefix, name),
                                                         std::get<GenericItem::AnyObject>(value).type().name(),
                                                         typeid(T).name())));

                return std::any_cast<const T &>(std::get<GenericItem::AnyObject>(value));
            }

            // Sub-search in the item if its name plus "//" matches the beginning of the var name
            if (DissolveSys::startsWith(varName, key + "//"))
            {
                auto dataName = varName;
                dataName.erase(0, key.length() + 2);
                auto optRef = GenericItemSearcher<T>::search(std::get<GenericItem::AnyObject>(value), dataName);
                if (optRef)
                    return optRef;
            }
        }

        return {};
    }
    // Convert supplied object reference to base-class reference
    template <class B, class T> const B &convertBase(const T &a, std::string_view varName) const
    {
        // Perform dynamic_cast to base class (B) and check
        OptionalReferenceWrapper<const B> base = dynamic_cast<const B &>(a);
        if (!base)
            throw(
                std::runtime_error(fmt::format("GenericList::convertBase() - Item named '{}' exists, but cannot be cast to the"
                                               "requested base type '{}' (item type is '{}').\n",
                                               varName, typeid(B).name(), typeid(T).name())));
        return base->get();
    }
    // Search for an object or child of the specified name and possible types, casting to the given base class
    template <class B, class T1, class T2>
    OptionalReferenceWrapper<const B> searchBase(std::string_view name, std::string_view prefix = "") const
    {
        auto varName = prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name);
        auto varNamePath = varName + "//";
        for (auto &[key, value] : items_)
        {
            // Match name
            if (varName == key)
            {
                auto &object = std::get<GenericItem::AnyObject>(value);
                OptionalReferenceWrapper<const B> optRef;
                if (object.type() == typeid(T1))
                    optRef = convertBase<B, T1>(std::any_cast<const T1 &>(object), varName);
                else if (object.type() == typeid(T2))
                    optRef = convertBase<B, T2>(std::any_cast<const T2 &>(object), varName);

                // If we have no valid result, then throw since the data type is not valid
                if (!optRef)
                    throw(std::runtime_error(
                        fmt::format("GenericList::searchBase() - Item named '{}' exists, but has a different "
                                    "type to those allowed requested ('{}' vs '{}' or '{}').\n",
                                    varName, object.type().name(), typeid(T1).name(), typeid(T2).name())));
                return optRef;
            }

            // Sub-search in the item if its name plus "//" matches the beginning of the var name
            if (DissolveSys::startsWith(varName, key + "//"))
            {
                auto dataName = varName;
                dataName.erase(0, key.length() + 2);
                auto &object = std::get<GenericItem::AnyObject>(value);

                OptionalReferenceWrapper<const T1> optRefT1 = GenericItemSearcher<const T1>::search(object, dataName);
                if (optRefT1)
                    return convertBase<B, T1>(optRefT1->get(), varName);

                OptionalReferenceWrapper<const T2> optRefT2 = GenericItemSearcher<const T2>::search(object, dataName);
                if (optRefT2)
                    return convertBase<B, T2>(optRefT2->get(), varName);
            }
        }

        return {};
    }
};
