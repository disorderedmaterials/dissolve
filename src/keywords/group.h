// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class KeywordList;

// Keyword Group
class KeywordGroup : public ListItem<KeywordGroup>
{
    public:
    KeywordGroup(KeywordList &keywordList);

    /*
     * Identity
     */
    private:
    // Name of the group
    std::string name_;

    public:
    // Set name of group
    void setName(std::string_view name);
    // Return name of group
    std::string_view name() const;

    /*
     * Keyword Group
     */
    private:
    // Associated KeywordList
    KeywordList &keywordList_;
    // List of keywords (in the referenced KeywordList) that are in this group
    RefList<KeywordBase> keywords_;

    private:
    // Add specified keyword to the group
    void addKeywordToGroup(KeywordBase *object);

    public:
    // Add keyword (pass-thru to KeywordList)
    bool add(KeywordBase *object, std::string_view keyword, std::string_view description,
             int optionMask = KeywordBase::NoOptions);
    // Add keyword (pass-thru to KeywordList)
    bool add(KeywordBase *object, std::string_view keyword, std::string_view description, std::string_view arguments,
             int optionMask = KeywordBase::NoOptions);
    // Return reference list of keywords in group
    RefList<KeywordBase> &keywords();
};
