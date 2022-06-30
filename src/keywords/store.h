// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/enumoptions.h"
#include "templates/optionalref.h"
#include <any>
#include <list>
#include <map>
#include <typeindex>
#include <unordered_map>

class SelectProcedureNode;
class Collect1DProcedureNode;
class RegionProcedureNodeBase;

// Keyword Store
class KeywordStore
{
    public:
    KeywordStore() = default;
    ~KeywordStore()
    {
        // Remove keywords in this store from the global reference
        auto it = std::remove_if(allKeywords_.begin(), allKeywords_.end(),
                                 [&](const auto *k)
                                 {
                                     for (auto &[name, keyword] : keywords_)
                                         if (k == keyword)
                                             return true;
                                     return false;
                                 });
        allKeywords_.erase(it, allKeywords_.end());
    }

    /*
     * Keyword Data
     */
    private:
    // Defined keywords
    std::map<std::string_view, KeywordBase *> keywords_;
    // Targets group
    std::vector<KeywordBase *> targetsGroup_;
    // Keyword group mappings
    std::vector<std::pair<std::string_view, std::vector<KeywordBase *>>> displayGroups_;
    // Keywords to be stored in the restart file
    std::vector<KeywordBase *> restartables_;

    public:
    // Add keyword (no group)
    template <class K, typename... Args>
    KeywordBase *addKeyword(std::string_view name, std::string_view description, Args &&...args)
    {
        // Check for keyword of this name already
        if (keywords_.find(name) != keywords_.end())
            throw(std::runtime_error(fmt::format("Keyword named '{}' already exists, and can't be added again.", name)));

        // Create new keyword using the supplied arguments
        K *k = new K(std::forward<Args>(args)...);
        k->setBaseInfo(name, description);

        // Add ourselves to both the local map and the global reference vector
        keywords_.emplace(name, k);
        allKeywords_.push_back(k);

        return k;
    }
    // Add target keyword
    template <class K, typename... Args> void addTarget(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        targetsGroup_.push_back(k);
    }
    // Add keyword (displaying in named group)
    template <class K, typename... Args>
    KeywordBase *add(std::string_view displayGroup, std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        auto it = std::find_if(displayGroups_.begin(), displayGroups_.end(),
                               [displayGroup](auto &group) { return group.first == displayGroup; });
        if (it == displayGroups_.end())
            displayGroups_.emplace_back(displayGroup, std::vector<KeywordBase *>{k});
        else
            it->second.push_back(k);

        return k;
    }
    // Add keyword (displaying in named group) and capture in restart file
    template <class K, typename... Args>
    KeywordBase *addRestartable(std::string_view displayGroup, std::string_view name, std::string_view description,
                                Args &&...args)
    {
        return restartables_.emplace_back(add<K>(displayGroup, name, description, args...));
    }
    // Find named keyword
    KeywordBase *find(std::string_view name);
    const KeywordBase *find(std::string_view name) const;
    // Return keywords
    const std::map<std::string_view, KeywordBase *> &keywords() const;
    // Return "Target" group keywords
    const std::vector<KeywordBase *> &targetsGroup() const;
    // Return restartable keywords
    const std::vector<KeywordBase *> &restartables() const;
    // Return keyword group mappings
    const std::vector<std::pair<std::string_view, std::vector<KeywordBase *>>> &displayGroups() const;

    /*
     * Set / Get
     */
    // Set specified keyword with supplied data
    void set(std::string_view name, const bool value);
    void set(std::string_view name, const double value);
    void set(std::string_view name, const int value);
    void set(std::string_view name, const std::string value);
    void set(std::string_view name, const Vec3<double> value);
    void set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value);
    void set(std::string_view name, const std::vector<std::shared_ptr<const Collect1DProcedureNode>> value);
    void set(std::string_view name, const std::shared_ptr<RegionProcedureNodeBase> value);
    void set(std::string_view name, const std::shared_ptr<SelectProcedureNode> value);
    void set(std::string_view name, const ConstNodeVector<SelectProcedureNode> value);
    void set(std::string_view name, const std::vector<Module *> value);
    void set(std::string_view name, const Module *value);
    void set(std::string_view name, Configuration *value);
    void set(std::string_view name, const std::vector<Configuration *> value);
    void set(std::string_view name, const Species *value);
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
    Configuration *getConfiguration(std::string_view name) const;
    std::vector<Configuration *> getVectorConfiguration(std::string_view name) const;
    const Species *getSpecies(std::string_view name) const;
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
    // Try to deserialise a single keyword through the specified LineParser
    KeywordBase::ParseResult deserialise(LineParser &parser, const CoreData &coreData, int startArg = 0);
    // Write all keywords to specified LineParser
    bool serialise(LineParser &parser, std::string_view prefix, bool onlyIfSet = true) const;

    /*
     * Object Management
     */
    private:
    // Vector of all keywords globally
    static std::vector<KeywordBase *> allKeywords_;

    public:
    // Gracefully deal with the specified object no longer being valid
    template <class O> static void objectNoLongerValid(O *object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto *kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }
};
