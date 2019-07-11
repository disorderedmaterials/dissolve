/*
	*** Character String List
	*** src/base/charstringlist.h
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

#ifndef DISSOLVE_CHARSTRINGLIST_H
#define DISSOLVE_CHARSTRINGLIST_H

#include "base/charstring.h"
#include "templates/list.h"

// Character String List
class CharStringList
{
	public:
	// Constructor / Destructor
	CharStringList();
	~CharStringList();
	// Conversion operator
	operator const List<CharString>&() const;
	// Copy constructor
	CharStringList(const CharStringList& source);
	// Conversion from const char*
	CharStringList(const char* text);
	// Assignment operator
	void operator=(const CharStringList& source);



	/*
	 * Data
	 */
	private:
	// List of CharStrings
	List<CharString> strings_;

	public:
	// Clear list
	void clear();
	// Add string to list
	void add(const char* s);
	// Return number of strings in list
	int nItems() const;
	// Return whether specified string is currently in the list
	bool contains(const char* s, bool caseSensitive = false) const;
	// Return list as comma-separated string
	const char* get() const;
};

#endif
