// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/enumoptions.h"
#include "templates/optionalref.h"
#include <any>
#include <list>
#include <map>
#include <typeindex>

// Keyword TypeMap
class KeywordTypeMap
{
    public:
    KeywordTypeMap();
    ~KeywordTypeMap() = default;

    private:
    // Function typedefs
    using SetterFunction = std::function<bool(KeywordBase *keyword, const std::any data)>;
    using GetterFunction = std::function<const std::any(KeywordBase *keyword)>;
    // Setter function map
    std::unordered_map<std::type_index, SetterFunction> directMapSetter_;
    // Getter function maps
    std::unordered_map<std::type_index, GetterFunction> directMapGetter_;
    // Register direct setter for specific keyword / data type pair
    template <class D, class K> void registerDirectMapping()
    {
        // Check for existing data
        if (directMapSetter_.find(typeid(D)) != directMapSetter_.end())
            throw(std::runtime_error(fmt::format("Duplicate mapping registered for type '{}'\n", typeid(D).name())));

        directMapSetter_[typeid(D)] = [](KeywordBase *keyword, const std::any data) {
            auto *k = dynamic_cast<K *>(keyword);
            assert(k);
            k->data() = std::any_cast<D>(data);
            k->setAsModified();
            return true;
        };
        directMapGetter_[typeid(D)] = [](KeywordBase *keyword) {
            auto *k = dynamic_cast<K *>(keyword);
            assert(k);
            return k->data();
        };
    }
    // Register direct setter for specific keyword / data type pair, with specific data setter on the keyword
    template <class D, class K> void registerDirectMapping(std::function<bool(K *keyword, const D data)> setFunction)
    {
        directMapSetter_[typeid(D)] = [setFunction](KeywordBase *keyword, const std::any &data) {
            auto *k = dynamic_cast<K *>(keyword);
            assert(k);
            return setFunction(k, std::any_cast<D>(data));
        };
    }

    public:
    // Set keyword data
    bool set(KeywordBase *keyword, const std::any data) const;
    // Get keyword data
    template <class D> D get(KeywordBase *keyword) const
    {
        // Find a suitable getter and call it
        auto it = directMapGetter_.find(typeid(D));
        if (it == directMapGetter_.end())
            throw(std::runtime_error(fmt::format(
                "Item of type '{}' cannot be returned as no suitable type mapping has been registered.\n", typeid(D).name())));

        auto data = (it->second)(keyword);
        return std::any_cast<D>(data);
    }
};

// Keyword List
class KeywordList
{
    public:
    KeywordList() = default;
    ~KeywordList() = default;

    /*
     * Keyword Data
     */
    private:
    // Defined keywords
    std::map<std::string_view, KeywordBase *> keywords_;
    // Keyword group mappings
    std::map<std::string_view, std::vector<std::string_view>> displayGroups_;

    public:
    // Add keyword (no group)
    template <class K, typename... Args>
    KeywordBase *addKeyword(std::string_view name, std::string_view description, Args &&... args)
    {
        // Check for keyword of this name already
        if (keywords_.find(name) != keywords_.end())
            throw(std::runtime_error(fmt::format("Keyword named '{}' already exists, and can't be added again.", name)));

        // Create new keyword using the supplied arguments
        K *k = new K(std::forward<Args>(args)...);
        k->setBaseInfo(name, description);

        keywords_.emplace(name, k);
        return k;
    }
    // Add keyword (displaying in named group)
    template <class K, typename... Args>
    KeywordBase *add(std::string_view displayGroup, std::string_view name, std::string_view description, Args &&... args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        displayGroups_[displayGroup].push_back(name);

        return k;
    }
    // Find named keyword
    KeywordBase *find(std::string_view name);
    const KeywordBase *find(std::string_view name) const;
    // Return keywords
    const std::map<std::string_view, KeywordBase *> keywords() const;
    // Return keyword group mappings
    const std::map<std::string_view, std::vector<std::string_view>> displayGroups() const;
    // Return whether the keyword has been set, and is not currently empty (if relevant)
    bool hasBeenSet(std::string_view name) const;
    // Flag that the specified keyword has been set by some external means
    void setAsModified(std::string_view name) const;

    /*
     * Set / Get
     */
    private:
    // Return the setter instance
    static const KeywordTypeMap &setters();

    public:
    // Set specified keyword with supplied data
    void set(std::string_view name, const std::any value);
    // Set specified keyword with supplied, template-guided data
    template <class D> bool set(std::string_view name, const D value)
    {
        auto it = keywords_.find(name);
        if (it == keywords_.end())
            throw(std::runtime_error(fmt::format("Keyword '{}' cannot be set as it doesn't exist.\n", name)));

        return setters().set(it->second, value);
    }
    // Set specified enumerated keyword
    template <class E> void setEnumeration(std::string_view name, E data)
    {
        auto it = keywords_.find(name);
        if (it == keywords_.end())
            throw(std::runtime_error(
                fmt::format("Enumerated keyword '{}' cannot be set as no suitable setter has been registered.\n", name)));

        auto *k = dynamic_cast<EnumOptionsKeyword<E> *>(it->second);
        if (!k)
            throw(std::runtime_error(
                fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(EnumOptionsKeyword<E>).name())));

        k->data() = data;
    }
    // Get specified keyword data, casting as necessary
    template <class D> D get(std::string_view name) const
    {
        auto it = keywords_.find(name);
        if (it == keywords_.end())
            throw(std::runtime_error(fmt::format("Data for keyword '{}' cannot be retrieved as it doesn't exist..\n", name)));

        return setters().get<D>(it->second);
    }
    // Get specified keyword data, casting as necessary
    template <class D, class K> OptionalReferenceWrapper<D> get(std::string_view name) const
    {
        auto it = keywords_.find(name);
        if (it == keywords_.end())
            return {};

        // Cast the keyword
        K *keyword = dynamic_cast<K *>(it->second);
        if (!keyword)
            throw(std::runtime_error(fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(K).name())));

        return keyword->data();
    }
    // Get specified keyword enumeration, casting as necessary
    template <class E> std::optional<E> getEnumeration(std::string_view name) const
    {
        auto it = keywords_.find(name);
        if (it == keywords_.end())
            return {};

        // Cast the keyword
        auto *keyword = dynamic_cast<EnumOptionsKeyword<E> *>(it->second);
        if (!keyword)
            throw(std::runtime_error(
                fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(EnumOptionsKeyword<E>).name())));

        return keyword->data();
    }

    /*
     * Read / Write
     */
    public:
    // Try to parse a single keyword through the specified LineParser
    KeywordBase::ParseResult parse(LineParser &parser, const CoreData &coreData, int startArg = 0);
    // Write all keywords to specified LineParser
    bool write(LineParser &parser, std::string_view prefix, bool onlyIfSet = true) const;
};
