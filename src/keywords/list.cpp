/*
	*** Keyword List
	*** src/keywords/list.cpp
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

#include "keywords/list.h"
#include "keywords/linkto.h"
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

// Add keyword (including argument description)
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

// Add keyword to named group (including argument description)
bool KeywordList::add(const char* groupName, KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask)
{
	KeywordGroup* group = addGroup(groupName);
	return group->add(object, name, description, arguments, optionMask);
}

// Add link to specified keyword that exists elsewhere
bool KeywordList::link(const char* groupName, KeywordBase* object, const char* name, const char* description, int optionMask)
{
	if (!object) return Messenger::error("NULL KeywordBase* passed to KeywordList::link().\n");

	// Create a new LinkToKeyword
	return add(groupName, new LinkToKeyword(object), name, description, optionMask);
}

// Add link to specified keyword that exists elsewhere (including argument description)
bool KeywordList::link(const char* groupName, KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask)
{
	if (!object) return Messenger::error("NULL KeywordBase* passed to KeywordList::link().\n");

	// Create a new LinkToKeyword
	return add(groupName, new LinkToKeyword(object), name, description, arguments, optionMask);
}

// Find named keyword
KeywordBase* KeywordList::find(const char* name) const
{
	for (KeywordBase* kwd = keywords_.first(); kwd != NULL; kwd = kwd->next()) if (DissolveSys::sameString(name, kwd->name())) return kwd->base();

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
		Messenger::warn("No Module keyword named '%s' exists to check whether it is set. Returning 'false'...\n", name);
		return false;
	}

	return keyword->isSet();
}

// Flag that the specified keyword has been set by some external means
void KeywordList::hasBeenSet(const char* name)
{
	// Find the named keyword
	KeywordBase* keyword = find(name);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to check whether it is set. Returning 'false'...\n", name);
		return;
	}

	keyword->hasBeenSet();
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

// Write all keywords to specified LineParser
bool KeywordList::write(LineParser& parser, const char* prefix, bool onlyIfSet)
{
	ListIterator<KeywordBase> keywordIterator(keywords_);
	while (KeywordBase* keyword = keywordIterator.iterate())
	{
		// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
		if (onlyIfSet && (!keyword->base()->isSet())) continue;

		// Make sure we are calling the write() function of the base() keyword class, but with the parent object's name()...
		if (!keyword->base()->write(parser, keyword->name(), prefix)) return false;
	}

	return true;
}

// Write all keywords in groups to specified LineParser
bool KeywordList::writeGroups(LineParser& parser, const char* prefix, bool onlyIfSet)
{
	// Loop over keyword groups
	bool firstGroup = true;
	ListIterator<KeywordGroup> groupsIterator(groups_);
	while (KeywordGroup* group = groupsIterator.iterate())
	{
		// Loop over keywords in group
		bool firstWritten = true;
		for (KeywordBase* keyword : group->keywords())
		{
			// If the keyword has never been set (i.e. it still has its default value) don't bother to write it
			if (onlyIfSet && (!keyword->base()->isSet())) continue;

			// If this is the first keyword to be written in the group, write the group name first as a comment
			if (firstWritten)
			{
				// If this is *not* the first group to be written, write a newline for formatting
				if ((!firstGroup) && (!parser.writeLineF("\n"))) return false;

				if (!parser.writeLineF("%s# %s\n", prefix, group->name())) return false;
			}

			// Make sure we are calling the write() function of the base() keyword class...
			if (!keyword->base()->write(parser, keyword->name(), prefix)) return false;

			// Falsify flags
			firstWritten = false;
			firstGroup = false;
		}
	}

	return true;
}
