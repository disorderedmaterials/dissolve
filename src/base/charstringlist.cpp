/*
	*** Character String List
	*** src/base/charstringlist.cpp
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

#include "base/charstringlist.h"
#include "base/sysfunc.h"

// Constructor
CharStringList::CharStringList()
{
}

// Destructor
CharStringList::~CharStringList()
{
}

// Conversion operator
CharStringList::operator const List<CharString>&() const
{
	return strings_;
}

// Copy constructor
CharStringList::CharStringList(const CharStringList& source)
{
	(*this) = source;
}

// Assignment operator
void CharStringList::operator=(const CharStringList& source)
{
	clear();

	for (CharString* s = source.strings_.first(); s != NULL; s = s->next) add(s->get());
}

/*
 * Data
 */

// Clear
void CharStringList::clear()
{
	strings_.clear();
}

// Add string to list
void CharStringList::add(const char* s)
{
	CharString* newString = strings_.add();
	newString->set(s);
}

// Return number of strings in list
int CharStringList::nItems() const
{
	return strings_.nItems();
}

// Return whether specified string is currently in the list
bool CharStringList::contains(const char* s, bool caseSensitive) const
{
	ListIterator<CharString> stringIterator(strings_);
	while (CharString* str = stringIterator.iterate()) if (DissolveSys::sameString(str->get(), s, caseSensitive)) return true;

	return false;
}

// Return list as comma-separated string
const char* CharStringList::get() const
{
	static CharString result;
	result.clear();
	ListIterator<CharString> stringIterator(strings_);
	while (CharString* str = stringIterator.iterate())
	{
		if (stringIterator.isFirst()) result = str->get();
		else result.strcatf(", %s", str->get());
	};

	return result.get();
}
