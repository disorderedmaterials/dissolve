/*
	*** Keyword List
	*** src/module/keywordlist.cpp
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

#include "keywords/list.h"
#include "base/sysfunc.h"

// Constructor
KeywordList::KeywordList()
{
}

// Destructor
KeywordList::~KeywordList()
{
}

/*
 * Keyword List
 */

// Add keyword
bool KeywordList::add(KeywordBase* object, const char* keyword, const char* description, int genericItemFlags)
{
	return add(object, keyword, description, "", genericItemFlags);
}

// Add keyword
bool KeywordList::add(KeywordBase* object, const char* keyword, const char* description, const char* arguments, int genericItemFlags)
{
	// Take ownership of the passed object, and set its basic information
	keywords_.own(object);
	object->set(keyword, description, arguments, genericItemFlags);

	return true;
}

// Find named keyword
KeywordBase* KeywordList::find(const char* keyword) const
{
	for (KeywordBase* kwd = keywords_.first(); kwd != NULL; kwd = kwd->next) if (DissolveSys::sameString(keyword, kwd->keyword())) return kwd;

	return NULL;
}

// Return first keyword in list
List<KeywordBase>& KeywordList::keywords()
{
	return keywords_;
}

/*
 * Conversion
 */

// Return simple keyword value (as bool)
bool KeywordList::asBool(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a bool. Returning 'false'...\n", keywordName);
		return false;
	}

	return keyword->asBool();
}

// Return simple keyword value (as int)
int KeywordList::asInt(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as an int. Returning '0'...\n", keywordName);
		return 0;
	}

	return keyword->asInt();
}

// Return simple keyword value (as double)
double KeywordList::asDouble(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a double. Returning '0.0'...\n", keywordName);
		return 0.0;
	}

	return keyword->asDouble();
}

// Return simple keyword value (as string)
const char* KeywordList::asString(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a string. Returning 'NULL'...\n", keywordName);
		return "NULL";
	}

	return keyword->asString();
}

// Return simple keyword value (as Vec3<int>)
Vec3<int> KeywordList::asVec3Int(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a Vec3<int>. Returning '(0,0,0)'...\n", keywordName);
		return Vec3<int>(0,0,0);
	}

	return keyword->asVec3Int();
}

// Return simple keyword value (as Vec3<double>)
Vec3<double> KeywordList::asVec3Double(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a Vec3<double>. Returning '(0.0,0.0,0.0)'...\n", keywordName);
		return Vec3<double>(0.0,0.0,0.0);
	}

	return keyword->asVec3Double();
}

// Return whether the specified keyword data has ever been set
bool KeywordList::isSet(const char* keywordName) const
{
	// Find the named keyword
	KeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to probe for its default value. Returning 'false'...\n", keywordName);
		return false;
	}

	return keyword->isSet();
}
