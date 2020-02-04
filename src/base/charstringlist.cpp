/*
	*** Character String List
	*** src/base/charstringlist.cpp
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

// Copy constructor
CharStringList::CharStringList(const CharStringList& source)
{
	(*this) = source;
}

// Conversion from const char*
CharStringList::CharStringList(const char* text)
{
	strings_.add(text);
}

// Assignment operator
void CharStringList::operator=(const CharStringList& source)
{
	clear();

	strings_ = source.strings_;
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
	strings_.add(s);
}

// Return number of strings in list
int CharStringList::nItems() const
{
	return strings_.nItems();
}

// Return nth string in list
const char* CharStringList::at(int index) const
{
	return strings_.constAt(index);
}

// Return whether specified string is currently in the list
bool CharStringList::contains(const char* s, bool caseSensitive) const
{
	for (int n=0; n<strings_.nItems(); ++n) if (DissolveSys::sameString(strings_.constAt(n), s, caseSensitive)) return true;

	return false;
}

// Return list as comma-separated string
const char* CharStringList::asCommaSeparatedList() const
{
	static CharString result;

	result.clear();

	for (int n=0; n<strings_.nItems(); ++n)
	{
		if (n == 0) result = strings_.constAt(n);
		else result.strcatf(", %s", strings_.constAt(n).get());
	}

	return result.get();
}
