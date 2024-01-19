// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/organiser.h"
#include <numeric>

/*
 * Keyword Store Group
 */

KeywordStoreGroup::KeywordStoreGroup(std::string_view name, std::optional<std::string_view> groupDescription) : name_(name)
{
    if (groupDescription)
        description_ = *groupDescription;
}

// Add keyword to group
void KeywordStoreGroup::addKeyword(KeywordBase *keyword, KeywordBase::KeywordType type)
{
    keywords_.emplace_back(keyword, type);
}

// Return the group name
std::string_view KeywordStoreGroup::name() const { return name_; }

// Return the group description
std::string_view KeywordStoreGroup::description() const { return description_; }

// Return the keywords vector
const std::vector<KeywordStoreEntry> &KeywordStoreGroup::keywords() const { return keywords_; }

// Find named keyword
std::optional<KeywordStoreEntry> KeywordStoreGroup::find(std::string_view name) const
{
    auto keywordIt =
        std::find_if(keywords_.begin(), keywords_.end(), [name](const auto &kd) { return kd.first->name() == name; });
    if (keywordIt != keywords_.end())
        return *keywordIt;

    return {};
}

/*
 * Keyword Store Section
 */

KeywordStoreSection::KeywordStoreSection(std::string_view name) : name_(name) { groups_.emplace_back("_NO_HEADER"); }

// Return section name
std::string_view KeywordStoreSection::name() const { return name_; }

// Get named group if it exists
OptionalReferenceWrapper<KeywordStoreGroup> KeywordStoreSection::getGroup(std::string_view groupName,
                                                                          std::optional<std::string_view> groupDescription,
                                                                          bool createIfRequired)
{
    auto groupIt = std::find_if(groups_.begin(), groups_.end(), [groupName](auto &group) { return group.name() == groupName; });
    if (groupIt != groups_.end())
        return *groupIt;

    if (createIfRequired)
        return groups_.emplace_back(groupName, groupDescription);

    return {};
}

// Return vector if defined groups
const std::vector<KeywordStoreGroup> &KeywordStoreSection::groups() const { return groups_; }

// Return number of keywords defined over all groups
int KeywordStoreSection::nKeywords() const
{
    return std::accumulate(groups_.begin(), groups_.end(), 0,
                           [](const auto acc, const auto &group) { return acc + group.keywords().size(); });
}

// Find named keyword
std::optional<KeywordStoreEntry> KeywordStoreSection::find(std::string_view name) const
{
    for (const auto &group : groups_)
    {
        auto optK = group.find(name);
        if (optK)
            return *optK;
    }

    return {};
}
