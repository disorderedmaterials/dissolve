// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "keywords/enumOptions.h"
#include "math/function1D.h"
#include "math/range.h"
#include "procedure/nodeValue.h"
#include "templates/optionalRef.h"
#include <any>
#include <map>
#include <typeindex>

// Keywords are organised into sections and groups for the purposes of display in the GUI, where:
// - The section represents a top-level button the UI (e.g. "Options", "Export", "Advanced") which has
//   associated with it one or more keywords organised into one or more named groups.
// - Groups represent logical ordering of keywords together within a section, e.g. step sizes or targets

// Type Definition for Basic Keyword Entry
using KeywordStoreEntry = std::pair<KeywordBase *, KeywordBase::KeywordType>;

// Keyword Store Group
class KeywordStoreGroup
{
    public:
    KeywordStoreGroup(std::string_view groupName, std::optional<std::string_view> groupDescription = {});

    private:
    // Group name and description
    std::string name_, description_;
    // Contained keywords
    std::vector<KeywordStoreEntry> keywords_;

    public:
    // Add keyword to group
    void addKeyword(KeywordBase *keyword, KeywordBase::KeywordType type);
    // Return the group name
    std::string_view name() const;
    // Return the group description
    std::string_view description() const;
    // Return the keywords vector
    const std::vector<KeywordStoreEntry> &keywords() const;

    /*
     * Find
     */
    public:
    // Find named keyword
    std::optional<KeywordStoreEntry> find(std::string_view name) const;
    // Find all keywords of specified type
    template <class K> std::vector<K *> allOfType() const
    {
        std::vector<K *> result;
        for (auto &&[k, kType] : keywords_)
            if (k->typeIndex() == typeid(K *))
                result.push_back(dynamic_cast<K *>(k));
        return result;
    }
};

// Keyword Store Section
class KeywordStoreSection
{
    public:
    KeywordStoreSection(std::string_view name);

    private:
    // Section name
    std::string name_;
    // Vector of defined keyword groups
    std::vector<KeywordStoreGroup> groups_;

    public:
    // Return section name
    std::string_view name() const;
    // Get named group if it exists
    OptionalReferenceWrapper<KeywordStoreGroup>
    getGroup(std::string_view groupName, std::optional<std::string_view> groupDescription = {}, bool createIfRequired = false);
    // Return vector of defined groups
    const std::vector<KeywordStoreGroup> &groups() const;
    // Return number of keywords defined over all groups
    int nKeywords() const;

    /*
     * Find
     */
    public:
    // Find named keyword
    std::optional<KeywordStoreEntry> find(std::string_view name) const;
    // Find all keywords of specified type
    template <class K> std::vector<K *> allOfType() const
    {
        std::vector<K *> result;
        for (auto &group : groups_)
        {
            auto subResult = group.allOfType<K>();
            result.insert(result.end(), subResult.begin(), subResult.end());
        }
        return result;
    }
};
