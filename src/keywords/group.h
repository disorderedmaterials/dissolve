/*
    *** Keyword Group
    *** src/keywords/group.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
