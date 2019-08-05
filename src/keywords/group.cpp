/*
	*** Module Keyword Group
	*** src/module/keywordgroup.cpp
	Copyright T. Youngs 2012-2019

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

#include "keywords/group.h"
#include "keywords/list.h"

// Constructor
KeywordGroup::KeywordGroup(KeywordList& keywordList) : ListItem<KeywordGroup>(), keywordList_(keywordList)
{
}

/*
 * Identity
 */

// Set name of group
void KeywordGroup::setName(const char* name)
{
	name_ = name;
}

// Return name of group
const char* KeywordGroup::name() const
{
	return name_.get();
}

/*
 * Keyword Group
 */

// Add specified keyword to the group
void KeywordGroup::addKeywordToGroup(KeywordBase* object)
{
	keywords_.append(object);
}

// Add keyword (pass-thru to KeywordList)
bool KeywordGroup::add(KeywordBase* object, const char* keyword, const char* description, int genericItemFlags)
{
	if (!keywordList_.add(object, keyword, description, "", genericItemFlags)) return false;
	
	addKeywordToGroup(object);

	return false;
}

// Add keyword (pass-thru to KeywordList)
bool KeywordGroup::add(KeywordBase* object, const char* keyword, const char* description, const char* arguments, int genericItemFlags)
{
	if (!keywordList_.add(object, keyword, description, arguments, genericItemFlags)) return false;

	addKeywordToGroup(object);

	return false;
}

// Return first keyword in list
RefList<KeywordBase>& KeywordGroup::keywords()
{
	return keywords_;
}
