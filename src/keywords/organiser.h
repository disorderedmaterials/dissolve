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

// Keywords are organised into sections and groups for the purposes of display in the GUI, where:
// - The section represents a top-level button the UI (e.g. "Options", "Export", "Advanced") which has
//   associated with it one or more keywords organised into one or more named groups.
// - Groups represent logical ordering of keywords together within a section, e.g. step sizes or targets

// Keyword Store Group
class KeywordStoreGroup
{
    public:
    KeywordStoreGroup(std::string_view groupName, std::string_view groupDescription = "");

    private:
    // Group name and description
    std::string name_, description_;
    // Contained keywords
    std::vector<KeywordBase *> keywords_;

    public:
    // Add keyword to group
    void addKeyword(KeywordBase *keyword);
    // Return the group name
    std::string_view name() const;
    // Return the group description
    std::string_view description() const;
    // Return the keywords vector
    const std::vector<KeywordBase *> &keywords() const;
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
    // Get named group, creating if necessary
    KeywordStoreGroup &getGroup(std::optional<std::string_view> groupName,
                                std::optional<std::string_view> groupDescription = {});
    // Return vector if defined groups
    const std::vector<KeywordStoreGroup> groups() const;
};

// Keyword Store Organiser
class KeywordStoreOrganiser
{
    private:
    // Defined keyword sections
    std::vector<KeywordStoreSection> sections_;
    // Current section accepting keywords
    OptionalReferenceWrapper<KeywordStoreGroup> currentGroup_;
    // Hidden keywords, not present in any section/group
    std::vector<KeywordBase *> hiddenKeywords_;

    public:
    // Set current section / group for keyword addition, creating if necessary
    void setCurrent(std::string_view sectionName, std::optional<std::string_view> groupName = {},
                    std::optional<std::string_view> groupDescription = {});
    // Add keyword to current section / group
    void addKeywordToCurrentGroup(KeywordBase *keyword);
    // Add hidden keyword
    void addHiddenKeyword(KeywordBase *keyword);
    // Return defined sections
    const std::vector<KeywordStoreSection> sections() const;
};
