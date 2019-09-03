/*
	*** Keyword List
	*** src/keywords/list.h
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

#ifndef DISSOLVE_KEYWORD_LIST_H
#define DISSOLVE_KEYWORD_LIST_H

#include "keywords/data.h"
#include "keywords/group.h"
#include "base/enumoptions.h"
#include "templates/list.h"

// Forward Declarations
class Module;

// Keyword List
class KeywordList
{
	public:
	// Constructor / Destructor
	KeywordList();
	~KeywordList();


	/*
	 * Keyword List
	 */
	private:
	// List of defined keywords
	List<KeywordBase> keywords_;

	public:
	// Add keyword
	bool add(KeywordBase* object, const char* name, const char* description, int optionMask = KeywordBase::NoOptions);
	// Add keyword
	bool add(KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask = KeywordBase::NoOptions);
	// Add keyword to named group
	bool add(const char* groupName, KeywordBase* object, const char* name, const char* description, int optionMask = KeywordBase::NoOptions);
	// Add keyword to named group
	bool add(const char* groupName, KeywordBase* object, const char* name, const char* description, const char* arguments, int optionMask = KeywordBase::NoOptions);
	// Add link to specified keyword that exists elsewhere
	template <class D> bool link(const char* groupName, KeywordData<D>* object)
	{
		if (!object) return Messenger::error("NULL KeywordData<D>* passed to KeywordList::link().\n");

		return add(groupName, object, object->name(), object->description, object->arguments(), object->genericItemFlags());
	}
	// Find named keyword
	KeywordBase* find(const char* name) const;
	// Return keywords list
	const List<KeywordBase>& keywords() const;


	/*
	 * Groups
	 */
	private:
	// Keywords organised by group
	List<KeywordGroup> groups_;

	private:
	// Create and/or return named keyword group
	KeywordGroup* addGroup(const char* name);

	public:
	// Return defined groups
	const List<KeywordGroup>& groups() const;


	/*
	 * Set / Get
	 */
	public:
	// Retrieve named item from specified list as template-guided type
	template <class T> T& retrieve(const char* name, T defaultValue = T(), bool* found = NULL)
	{
		// Find item in the list
		KeywordBase* item = find(name);
		if (!item)
		{
			Messenger::printVerbose("No item named '%s' in the keyword list - default value item will be returned.\n", name);
			static T dummy;
			dummy = defaultValue;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		// Attempt to cast to specified type
		KeywordData<T>* castItem = dynamic_cast<KeywordData<T>*>(item);
		if (!castItem)
		{
			printf("That didn't work, because it's of the wrong type.\n");
			static T dummy;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		if (found != NULL) (*found) = true;
		return castItem->data();
	}
	// Set named item from specified list as a template-guided type
	template <class T> bool set(const char* name, T value)
	{
		// Find item in the list
		KeywordBase* item = find(name);
		if (!item)
		{
			Messenger::warn("No item named '%s' in the keyword list - cannot set it's value.\n", name);
			return false;
		}

		// Attempt to cast to specified type
		KeywordData<T>* castItem = dynamic_cast<KeywordData<T>*>(item);
		if (!castItem)
		{
			printf("That didn't work, because it's of the wrong type.\n");
			return false;
		}

		// Set the new value
		castItem->setData(value);

		return true;
	}
	// Set named EnumOptions from specified list as a template-guided type
	template <class E> bool setEnumeration(const char* name, E value)
	{
		// Find item in the list
		KeywordBase* item = find(name);
		if (!item)
		{
			Messenger::warn("No item named '%s' in the keyword list - cannot set it's value.\n", name);
			return false;
		}

		// Attempt to cast to specified type
		KeywordData< EnumOptions<E> >* castItem = dynamic_cast<KeywordData< EnumOptions<E> >*>(item);
		if (!castItem)
		{
			printf("That didn't work, because it's of the wrong type.\n");
			return false;
		}

		// Set the new value
		castItem->data() = value;

		return true;
	}
	// Retrieve named EnumOptions with specified class, and return its current enumeration
	template <class E> E enumeration(const char* name, bool* found = NULL)
	{
		// Find item in the list
		KeywordBase* item = find(name);
		if (!item)
		{
			Messenger::error("No item named '%s' in the keyword list - default enumeration of -1 will be returned.\n", name);
			if (found != NULL) (*found) = false;
			return (E) -1;
		}

		// Attempt to cast to EnumOptionsBase
		KeywordData< EnumOptions<E> >* castItem = dynamic_cast<KeywordData< EnumOptions<E> >*>(item);
		if (!castItem)
		{
			Messenger::error("Failed to cast keyword '%s' into EnumOptions<E> because it's of a different type.\n", name);
			if (found != NULL) (*found) = false;
			return (E) -1;
		}

		if (found != NULL) (*found) = true;
		return castItem->data().enumeration();
	}


	/*
	 * Conversion
	 */
	public:
	// Return simple keyword value (as bool)
	bool asBool(const char* name) const;
	// Return simple keyword value (as int)
	int asInt(const char* name) const;
	// Return simple keyword value (as double)
	double asDouble(const char* name) const;
	// Return simple keyword value (as string)
	const char* asString(const char* name) const;
	// Return simple keyword value (as Vec3<int>)
	Vec3<int> asVec3Int(const char* name) const;
	// Return simple keyword value (as Vec3<double>)
	Vec3<double> asVec3Double(const char* name) const;
	// Return whether the specified keyword data has ever been set
	bool isSet(const char* name) const;


	/*
	 * Read / Write
	 */
	public:
	// Try to parse keyword in specified LineParser
	KeywordBase::ParseResult parse(LineParser& parser, const CoreData& coreData);
};

#endif
