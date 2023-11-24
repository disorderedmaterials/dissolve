// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/organiser.h"

/*
 * Keyword Store Group
 */

KeywordStoreGroup::KeywordStoreGroup(std::string_view name, std::string_view description)
    : name_(name), description_(description)
{
}

// Add keyword to group
void KeywordStoreGroup::addKeyword(KeywordBase *keyword) { keywords_.push_back(keyword); }

// Return the group name
std::string_view KeywordStoreGroup::name() const { return name_; }

// Return the group description
std::string_view KeywordStoreGroup::description() const { return description_; }

// Return the keywords vector
const std::vector<KeywordBase *> &KeywordStoreGroup::keywords() const { return keywords_; }

/*
 * Keyword Store Section
 */

KeywordStoreSection::KeywordStoreSection(std::string_view name) : name_(name) { groups_.emplace_back("_NO_HEADER"); }

// Return section name
std::string_view KeywordStoreSection::name() const { return name_; }

// Get named group, creating if necessary
KeywordStoreGroup &KeywordStoreSection::getGroup(std::optional<std::string_view> groupName,
                                                 std::optional<std::string_view> groupDescription)
{
    // If no group name is supplied we return the _NO_HEADER group (front of the vector)
    if (!groupName)
        return groups_.front();

    // Return an existing group, or create a new one with the supplied name
    auto name = *groupName;
    auto groupIt = std::find_if(groups_.begin(), groups_.end(), [name](const auto &group) { return group.name() == name; });
    return groupIt == groups_.end() ? groups_.emplace_back(name, groupDescription.value_or("")) : *groupIt;
}

// Return vector if defined groups
const std::vector<KeywordStoreGroup> KeywordStoreSection::groups() const { return groups_; }

/*
 * Keyword Store Organiser
 */

// Set current section / group for keyword addition, creating if necessary
void KeywordStoreOrganiser::setCurrent(std::string_view sectionName, std::optional<std::string_view> groupName,
                                       std::optional<std::string_view> groupDescription)
{
    // Find the named section
    auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
                                  [sectionName](const auto &section) { return section.name() == sectionName; });
    auto &section = sectionIt == sections_.end() ? sections_.emplace_back(sectionName) : *sectionIt;

    // Get / create the group within the section
    currentGroup_ = section.getGroup(groupName, groupDescription);
}

// Add keyword to current section / group
void KeywordStoreOrganiser::addKeywordToCurrentGroup(KeywordBase *keyword)
{
    if (!currentGroup_)
        setCurrent("_DEFAULT_SECTION");

    currentGroup_->get().addKeyword(keyword);
}

// Return defined sections
const std::vector<KeywordStoreSection> KeywordStoreOrganiser::sections() const { return sections_; }
