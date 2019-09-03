/*
	*** Keyword List
	*** src/keywords/list.cpp
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
#include "base/lineparser.h"
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
bool KeywordList::add(KeywordBase* object, const char* name, const char* description, int optionMask)
{
	return add(object, name, description, "", optionMask);
}

// Add keyword
bool KeywordList::add(KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask)
{
	// Take ownership of the passed object, and set its basic information
	keywords_.own(object);
	object->set(name, description, arguments, optionMask);

	return true;
}

// Add keyword to named group
bool KeywordList::add(const char* groupName, KeywordBase* object, const char* name, const char* description, int optionMask)
{
	KeywordGroup* group = addGroup(groupName);
	return group->add(object, name, description, optionMask);
}

// Add keyword to named group
bool KeywordList::add(const char* groupName, KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask)
{
	KeywordGroup* group = addGroup(groupName);
	return group->add(object, name, description, arguments, optionMask);
}

// Find named keyword
KeywordBase* KeywordList::find(const char* name) const
{
	for (KeywordBase* kwd = keywords_.first(); kwd != NULL; kwd = kwd->next()) if (DissolveSys::sameString(name, kwd->base()->name())) return kwd;

	return NULL;
}

// Return first keyword in list
const List<KeywordBase>& KeywordList::keywords() const
{
	return keywords_;
}

/*
 * Groups
 */

// Create and/or return named keyword group
KeywordGroup* KeywordList::addGroup(const char* name)
{
	// Check that a group with the specified name doesn't already exist
	KeywordGroup* group = NULL;
	for (group = groups_.first(); group != NULL; group = group->next()) if (DissolveSys::sameString(name, group->name())) break;

	if (!group)
	{
		group = new KeywordGroup(*this);
		group->setName(name);
		groups_.own(group);
	}

	return group;
}

// Return defined groups
const List<KeywordGroup>& KeywordList::groups() const
{
	return groups_;
}

/*
 * Conversion
 */

// Return simple keyword value (as bool)
bool KeywordList::asBool(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a bool. Returning 'false'...\n", name);
		return false;
	}

	return keyword->asBool();
}

// Return simple keyword value (as int)
int KeywordList::asInt(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as an int. Returning '0'...\n", name);
		return 0;
	}

	return keyword->asInt();
}

// Return simple keyword value (as double)
double KeywordList::asDouble(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a double. Returning '0.0'...\n", name);
		return 0.0;
	}

	return keyword->asDouble();
}

// Return simple keyword value (as string)
const char* KeywordList::asString(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a string. Returning 'NULL'...\n", name);
		return "NULL";
	}

	return keyword->asString();
}

// Return simple keyword value (as Vec3<int>)
Vec3<int> KeywordList::asVec3Int(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a Vec3<int>. Returning '(0,0,0)'...\n", name);
		return Vec3<int>(0,0,0);
	}

	return keyword->asVec3Int();
}

// Return simple keyword value (as Vec3<double>)
Vec3<double> KeywordList::asVec3Double(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a Vec3<double>. Returning '(0.0,0.0,0.0)'...\n", name);
		return Vec3<double>(0.0,0.0,0.0);
	}

	return keyword->asVec3Double();
}

// Return whether the specified keyword data has ever been set
bool KeywordList::isSet(const char* name) const
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to probe for its default value. Returning 'false'...\n", name);
		return false;
	}

	return keyword->isSet();
}

/*
 * Read / Write
 */

// Try to parse node keyword in specified LineParser
KeywordBase::ParseResult KeywordList::parse(LineParser& parser, const CoreData& coreData)
{
	// Do we recognise the first item (the 'keyword')?
	KeywordBase* keyword = find(parser.argc(0));
	if (!keyword) return KeywordBase::Unrecognised;

	// We recognised the keyword - check the number of arguments we have against the min / max for the keyword
	if (!keyword->validNArgs(parser.nArgs() - 1)) return KeywordBase::Failed;

	// All OK, so parse the keyword
	if (!keyword->read(parser, 1, coreData))
	{
		Messenger::error("Failed to parse arguments for keyword '%s'.\n", keyword->name());
		return KeywordBase::Failed;
	}

	return KeywordBase::Success;
}
