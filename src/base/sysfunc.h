/*
	*** System Functions
	*** src/base/sysfunc.h
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

#ifndef DISSOLVE_SYSFUNC_H
#define DISSOLVE_SYSFUNC_H

// System Functions
class DissolveSys
{
	/*
	 * Conversion Functions
	 */
	public:
	// Convert integer to string representation
	static const char* itoa(int i);
	// Convert double to string representation (as %f)
	static const char* ftoa(double d);
	// Convert double to string representation with supplied format
	static const char* ftoa(double d, const char* fmt);
	// Convert string to boolean
	static bool atob(const char* s);
	// Convert boolean to string representation ("True" or "False")
	static const char* btoa(bool b);
	// Convert boolean to string representation ("On" or "Off")
	static const char* onOff(bool b);


	/*
	 * String Functions
	 */
	public:
	// Convert string to uppercase
	static const char* upperCase(const char*);
	// Convert string to lowercase
	static const char* lowerCase(const char*);
	// Perform case-(in)sensitive string comparison
	static bool sameString(const char* s1, const char* s2, bool caseSensitive = false);
	// Perform case-(in)sensitive, wildcard-enabled string comparison
	static bool sameWildString(const char* wild, const char* s2, bool caseSensitive = false);
	// Get characters before first occurrence of designated character
	static const char* beforeChar(const char*, char);
	// Get characters after first occurrence of designated character
	static const char* afterChar(const char*, char);
	// Get characters after last occurrence of designated character
	static const char* afterLastChar(const char*, char);
	// Get characters before last occurrence of designated character
	static const char* beforeLastChar(const char*, char);
	// Get characters before first occurrence of designated string
	static const char* beforeStr(const char*, const char*);
	// Get characters after first occurrence of designated character
	static const char* afterStr(const char*, const char*);
	// Remove comments from line
	static void removeComments(char* s);
	// Strip trailing whitespace from string
	static const char* stripTrailing(const char*);
	// Replace all of the supplied characters in the source string
	static const char* replaceChars(const char* s, const char* charstoreplace, char r);
	// Strip all of the supplied characters from the source string
	static const char* stripChars(const char* s, const char* charstostrip);
	// Count number of times that supplied characters occur in supplied string
	static int countChars(const char* string, const char* searchchars, int offset = 0);
	// Return whether string consists of empty whitespace characters only
	static bool isEmpty(const char* s);
	// Return nice version of string (no spaces, slashes etc.)
	static const char* niceName(const char* original);
	// Return whether the supplied string is a number
	static bool isNumber(const char* text);
	// Return whether the supplied string is a number, and also whether it is floating-point
	static bool isNumber(const char* text, bool& isFloatingPoint);


	/*
	 * Files
	 */
	public:
	// Return whether specified file exists
	static bool fileExists(const char* filename);


	/*
	 * Time Functions
	 */
	public:
	// Return string of current time / date
	static const char* currentTimeAndDate();
};

#endif
