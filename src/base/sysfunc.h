/*
	*** System functions
	*** src/base/sysfunc.h
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_SYSFUNC_H
#define DUQ_SYSFUNC_H

// System Functions
class DUQSys
{
	public:
	// Number/string conversion
	static const char* itoa(int);
	static const char* ftoa(double);
	static const char* ftoa(double, const char*);

	// String functions
	static const char* upperCase(const char*);
	static const char* lowerCase(const char*);
	static bool sameString(const char* s1, const char* s2, bool caseSensitive = false);
	static const char* beforeChar(const char*, char);
	static const char* afterChar(const char*, char);
	static const char* afterLastChar(const char*, char);
	static const char* beforeLastChar(const char*, char);
	static const char* beforeStr(const char*, const char*);
	static const char* afterStr(const char*, const char*);
	static const char* stripTrailing(const char*);
	static const char* replaceChars(const char* s, const char* charstoreplace, char r);
	static const char* stripChars(const char* s, const char* charstostrip);
	static int countChars(const char* string, const char* searchchars, int offset = 0);
	static void removeComments(char* s);
	static bool isEmpty(const char* s);

	// Enum search and print
	static int enumSearch(const char* name, int nitems, const char**list, const char* query, bool reportError = 1);
	static void enumPrintValid(int nitems, const char**list);

	// Files
	static bool fileExists(const char* filename);
};

#endif
