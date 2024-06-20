// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/enumOptions.h"
#include "keywords/organiser.h"
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
        // This is now a bit of a clumsy function since we do a separate remove/erase for each group.
        // This will be tidied up as part of #1637 (Remove Static Singletons)
        for (const auto &section : sections_)
            for (const auto &group : section.groups())
            {
                auto it = std::remove_if(allKeywords_.begin(), allKeywords_.end(),
                                         [&](const auto *k)
                                         {
                                             return std::find_if(group.keywords().begin(), group.keywords().end(),
                                                                 [k](const auto &kd)
                                                                 { return k == kd.first; }) != group.keywords().end();
                                         });
                allKeywords_.erase(it, allKeywords_.end());
            }
    }

    /*
     * Keyword Data
     */
    private:
    // Defined keyword sections
    std::vector<KeywordStoreSection> sections_;
    // Current section accepting keywords
    OptionalReferenceWrapper<KeywordStoreGroup> currentGroup_;

    private:
    // Create keyword
    template <class K, typename... Args>
    KeywordBase *createKeyword(std::string_view name, std::string_view description, Args &&...args)
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
    // Add keyword to current section / group
    void addKeywordToCurrentGroup(KeywordBase *keyword, KeywordBase::KeywordType type);
    // Return named group, if it exists, optionally creating it if it doesn't
    OptionalReferenceWrapper<KeywordStoreGroup> getGroup(std::string_view sectionName, std::string_view groupName,
                                                         std::optional<std::string_view> groupDescription = {},
                                                         bool createIfRequired = false);

    public:
    // Set current group and section organisation
    void setOrganisation(std::string_view sectionName, std::optional<std::string_view> groupName = {},
                         std::optional<std::string_view> groupDescription = {});
    // Add target keyword
    template <class K, typename... Args>
    KeywordBase *addTarget(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = createKeyword<K>(name, description, args...);

        auto optTargetsGroup = getGroup("Options", "Targets", {}, true);

        optTargetsGroup->get().addKeyword(k, KeywordBase::KeywordType::Target);

        return k;
    }
    // Add hidden keyword (no group)
    template <class K, typename... Args>
    KeywordBase *addHidden(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = createKeyword<K>(name, description, args...);

        auto optHiddenGroup = getGroup("Options", "_HIDDEN", {}, true);

        optHiddenGroup->get().addKeyword(k, KeywordBase::KeywordType::Target);

        return k;
    }
    // Add keyword, displaying in current section/group
    template <class K, typename... Args> KeywordBase *add(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = createKeyword<K>(name, description, args...);

        addKeywordToCurrentGroup(k, KeywordBase::KeywordType::Standard);

        return k;
    }
    // Add keyword (displaying in current section/group) and capture in restart file
    template <class K, typename... Args>
    KeywordBase *addRestartable(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = createKeyword<K>(name, description, args...);

        addKeywordToCurrentGroup(k, KeywordBase::KeywordType::Restartable);

        return k;
    }
    // Add deprecated keyword
    template <class K, typename... Args>
    KeywordBase *addDeprecated(std::string_view name, std::string_view description, Args &&...args)
    {
        auto *k = createKeyword<K>(name, description, args...);

        auto optHiddenGroup = getGroup("Options", "_HIDDEN", {}, true);

        optHiddenGroup->get().addKeyword(k, KeywordBase::KeywordType::Deprecated);

        return k;
    }
    // Find named keyword
    std::optional<KeywordStoreEntry> find(std::string_view name);
    std::optional<KeywordStoreEntry> find(std::string_view name) const;
    // Find all keywords of specified type
    template <class K> std::vector<K *> allOfType()
    {
        std::vector<K *> result;
        for (auto &section : sections_)
        {
            auto subResult = section.allOfType<K>();
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
        return result;
    }
    // Return all target keywords
    std::vector<KeywordBase *> targetKeywords();
    // Return defined keyword sections
    const std::vector<KeywordStoreSection> &sections() const;
    // Return number of visible keywords defined over all sections
    int nVisibleKeywords() const;

    /*
     * Set / Get
     */
    public:
    // Set specified keyword with supplied data
    bool set(std::string_view name, const bool value);
    bool set(std::string_view name, const double value);
    bool set(std::string_view name, const int value);
    bool set(std::string_view name, const std::string value);
    bool set(std::string_view name, const Function1DWrapper value);
    bool set(std::string_view name, const NodeValueProxy value);
    bool set(std::string_view name, const Vec3<double> value);
    bool set(std::string_view name, const Vec3<int> value);
    bool set(std::string_view name, const Vec3<NodeValue> value);
    bool set(std::string_view name, const Range value);
    bool set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value);
    bool set(std::string_view name, const std::vector<const SpeciesSite *> value);
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
        auto optKeyword = find(name);
        if (!optKeyword)
            throw(std::runtime_error(
                fmt::format("Enumerated keyword '{}' cannot be set as no suitable setter has been registered.\n", name)));

        auto *k = dynamic_cast<EnumOptionsKeyword<E> *>(optKeyword->first);
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
        auto optKeyword = find(name);
        if (!optKeyword)
            return {};

        // Cast the keyword
        const K *keyword = dynamic_cast<const K *>(optKeyword->first);
        if (!keyword)
            throw(std::runtime_error(fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(K).name())));

        return keyword->data();
    }
    template <class D, class K> std::optional<D> get(std::string_view name)
    {
        auto optKeyword = find(name);
        if (!optKeyword)
            return {};

        // Cast the keyword
        K *keyword = dynamic_cast<K *>(optKeyword->first);
        if (!keyword)
            throw(std::runtime_error(fmt::format("Couldn't cast keyword '{}' into type '{}'.\n", name, typeid(K).name())));

        return keyword->data();
    }
    // Get specified keyword enumeration, casting as necessary
    template <class E> std::optional<E> getEnumeration(std::string_view name) const
    {
        auto optKeyword = find(name);
        if (!optKeyword)
            return {};

        // Cast the keyword
        const auto *keyword = dynamic_cast<const EnumOptionsKeyword<E> *>(optKeyword->first);
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
    // Vector of all keywords in the store
    std::vector<KeywordBase *> allKeywords_;

    public:
    // Gracefully deal with the specified object no longer being valid
    template <class O> void objectNoLongerValid(O *object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto *kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }

    template <class P> void objectNoLongerValid(std::shared_ptr<P> object)
    {
        // Loop over all keyword objects and call their local functions
        for (auto *kwd : allKeywords_)
            kwd->removeReferencesTo(object);
    }
};
