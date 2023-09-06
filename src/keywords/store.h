// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/enumOptions.h"
#include "keywords/storeData.h"
#include "math/function1D.h"
#include "math/range.h"
#include "procedure/nodeValue.h"
#include "templates/optionalRef.h"
#include <any>
#include <map>
#include <typeindex>

class SelectProcedureNode;
class Collect1DProcedureNode;
class RegionProcedureNodeBase;
class SQModule;
class RDFModule;
class Configuration;

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
                                     for (auto &kd : keywords_)
                                         if (k == kd.keyword())
                                             return true;
                                     return false;
                                 });
        allKeywords_.erase(it, allKeywords_.end());
    }

    /*
     * Keyword Data
     */
    private:
    // Keywords present in this store
    std::vector<KeywordStoreData> keywords_;
    // Current group and section organisation for new keywords
    std::string currentGroupName_{"UNGROUPED"}, currentSectionName_;

    private:
    // Add keyword
    template <class K, typename... Args>
    KeywordBase *addKeyword(std::string_view name, std::string_view description, Args &&...args)
    {
        // Check for keyword of this name already
        if (find(name))
            throw(std::runtime_error(fmt::format("Keyword named '{}' already exists, and can't be added again.", name)));

        // Create new keyword using the supplied arguments
        K *k = new K(std::forward<Args>(args)...);
        k->setBaseInfo(name, description);

        // Add the new keyword to the global reference vector
        allKeywords_.push_back(k);

        return k;
    }

    public:
    // Set current group and section organisation
    void setOrganisation(std::string_view groupName, std::string_view sectionName = "");
    // Add target keyword
    template <class K, typename... Args> void addTarget(std::string_view name, std::string_view description, Args &&...args)
    {
        keywords_.emplace_back(addKeyword<K>(name, description, args...), KeywordStoreData::KeywordType::Target, "Options",
                               "Targets");
    }
    // Add hidden keyword (no group)
    template <class K, typename... Args>
    KeywordBase *addHidden(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        keywords_.emplace_back(k, KeywordStoreData::KeywordType::Standard);

        return k;
    }
    // Add keyword, displaying in named group
    template <class K, typename... Args> KeywordBase *add(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        keywords_.emplace_back(k, KeywordStoreData::KeywordType::Standard, currentGroupName_, currentSectionName_);

        return k;
    }
    // Add keyword (displaying in named group) and capture in restart file
    template <class K, typename... Args>
    KeywordBase *addRestartable(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        keywords_.emplace_back(k, KeywordStoreData::KeywordType::Restartable, currentGroupName_, currentSectionName_);

        return k;
    }
    // Add deprecated keyword
    template <class K, typename... Args>
    KeywordBase *addDeprecated(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = addKeyword<K>(name, description, args...);

        keywords_.emplace_back(k, KeywordStoreData::KeywordType::Deprecated);

        return k;
    }
    // Find named keyword
    OptionalReferenceWrapper<KeywordStoreData> find(std::string_view name);
    OptionalReferenceWrapper<const KeywordStoreData> find(std::string_view name) const;
    // Find all keywords of specified type
    template <class K> std::vector<K *> allOfType()
    {
        std::vector<K *> result;
        for (auto &kd : keywords_)
            if (kd.keyword()->typeIndex() == typeid(K *))
                result.push_back(dynamic_cast<K *>(kd.keyword()));
        return result;
    }
    // Return keywords
    const std::vector<KeywordStoreData> &keywords() const;
    // Return all target keywords
    std::vector<KeywordBase *> targetKeywords();
    // Return keyword organisation based on group and section names
    using KeywordStoreIndexInfo = std::pair<std::string_view, std::vector<std::string_view>>;
    using KeywordStoreIndex = std::vector<KeywordStoreIndexInfo>;
    using KeywordStoreMap = std::map<std::string_view, std::map<std::string_view, std::vector<KeywordBase *>>>;
    std::pair<KeywordStoreIndex, KeywordStoreMap> keywordOrganisation();

    /*
     * Set / Get
     */
    public:
    // Set specified keyword with supplied data
    bool set(std::string_view name, const bool value);
    bool set(std::string_view name, const double value);
    bool set(std::string_view name, const int value);
    bool set(std::string_view name, const std::string value);
    bool set(std::string_view name, const Functions::Function1DWrapper value);
    bool set(std::string_view name, const NodeValueProxy value);
    bool set(std::string_view name, const Vec3<double> value);
    bool set(std::string_view name, const Vec3<int> value);
    bool set(std::string_view name, const Vec3<NodeValue> value);
    bool set(std::string_view name, const Range value);
    bool set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value);
    bool set(std::string_view name, const std::vector<std::shared_ptr<const Collect1DProcedureNode>> value);
    bool set(std::string_view name, const std::shared_ptr<RegionProcedureNodeBase> value);
    bool set(std::string_view name, const std::shared_ptr<SelectProcedureNode> value);
    bool set(std::string_view name, const ConstNodeVector<SelectProcedureNode> value);
    bool set(std::string_view name, const std::vector<Module *> value);
    bool set(std::string_view name, const Module *value);
    bool set(std::string_view name, Configuration *value);
    bool set(std::string_view name, const std::vector<Configuration *> value);
    bool set(std::string_view name, const Species *value);
    // Set specified enumerated keyword
    template <class E> void setEnumeration(std::string_view name, E data)
    {
        auto kd = find(name);
        if (!kd)
            throw(std::runtime_error(
                fmt::format("Enumerated keyword '{}' cannot be set as no suitable setter has been registered.\n", name)));

        auto *k = dynamic_cast<EnumOptionsKeyword<E> *>(kd->get().keyword());
        if (!k)
            throw(std::runtime_error(
                fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(EnumOptionsKeyword<E>).name())));

        k->data() = data;
    }

    /*
     * Getters for the Keyword Store.  This should eventually be
     * replaced with a single get function.  The problem is two-fold.
     * First, C++ doesn't allow for overloading on return-type, so we
     * need a template that takes the return type as a parameter.  That
     * template can then be specialised to the individual return types,
     * with the generic implementation throwing an exception.  Now C++11
     * doesn't allow template specialisation within a member function.
     * C++14 added support for this, but GCC still does not support this
     * as of GCC 12.
     */

    // Retrieve a Configuration by keyword name
    Configuration *getConfiguration(std::string_view name) const;
    // Retrieve a vector of Configurations by keyword name
    std::vector<Configuration *> getVectorConfiguration(std::string_view name) const;
    // Retrieve a Species by keyword name
    const Species *getSpecies(std::string_view name) const;
    // Retrieve a vector of Modules by keyword name
    const std::vector<Module *> &getVectorModule(std::string_view name) const;
    // Retrieve an Integer by keyword name
    int getInt(std::string_view name) const;

    // Get specified keyword data, casting as necessary
    template <class D, class K> std::optional<const D> get(std::string_view name) const
    {
        auto optKd = find(name);
        if (!optKd)
            return {};

        // Cast the keyword
        const K *keyword = dynamic_cast<const K *>(optKd->get().keyword());
        if (!keyword)
            throw(std::runtime_error(fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(K).name())));

        return keyword->data();
    }
    template <class D, class K> std::optional<D> get(std::string_view name)
    {
        auto optKd = find(name);
        if (!optKd)
            return {};

        // Cast the keyword
        K *keyword = dynamic_cast<K *>(optKd->get().keyword());
        if (!keyword)
            throw(std::runtime_error(fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(K).name())));

        return keyword->data();
    }
    // Get specified keyword enumeration, casting as necessary
    template <class E> std::optional<E> getEnumeration(std::string_view name) const
    {
        auto optKd = find(name);
        if (!optKd)
            return {};

        // Cast the keyword
        const auto *keyword = dynamic_cast<const EnumOptionsKeyword<E> *>(optKd->get().keyword());
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
    // Apply the terms in the keyword store to a node
    SerialisedValue serialiseOnto(SerialisedValue node) const;
    // Pull keywords from entries in table
    void deserialiseFrom(const SerialisedValue &node, const CoreData &coreData);

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
