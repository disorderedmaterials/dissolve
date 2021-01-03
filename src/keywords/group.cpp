// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/group.h"
#include "keywords/list.h"

KeywordGroup::KeywordGroup(KeywordList &keywordList) : ListItem<KeywordGroup>(), keywordList_(keywordList) {}

/*
 * Identity
 */

// Set name of group
void KeywordGroup::setName(std::string_view name) { name_ = name; }

// Return name of group
std::string_view KeywordGroup::name() const { return name_; }

/*
 * Keyword Group
 */

// Add specified keyword to the group
void KeywordGroup::addKeywordToGroup(KeywordBase *object) { keywords_.append(object); }

// Add keyword (pass-thru to KeywordList)
bool KeywordGroup::add(KeywordBase *object, std::string_view keyword, std::string_view description, int optionMask)
{
    if (!keywordList_.add(object, keyword, description, "", optionMask))
        return false;

    addKeywordToGroup(object);

    return false;
}

// Add keyword (pass-thru to KeywordList)
bool KeywordGroup::add(KeywordBase *object, std::string_view keyword, std::string_view description, std::string_view arguments,
                       int optionMask)
{
    if (!keywordList_.add(object, keyword, description, arguments, optionMask))
        return false;

    addKeywordToGroup(object);

    return false;
}

// Return first keyword in list
RefList<KeywordBase> &KeywordGroup::keywords() { return keywords_; }
