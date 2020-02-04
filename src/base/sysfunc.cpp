/*
	*** System Functions
	*** src/base/sysfunc.cpp
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

#include "base/sysfunc.h"
#include "base/messenger.h"
#include "base/charstring.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <string.h>
#include <ctype.h>

using namespace std;

/*
 * Conversion Functions
 */

// Convert integer to string representation
const char* DissolveSys::itoa(int n)
{
	static CharString result;
	result.sprintf("%i",n);
	return result;
}

// Convert double to string representation (as %f)
const char* DissolveSys::ftoa(double f)
{
	static CharString result;
	result.sprintf("%f",f);
	return result;
}

// Convert double to string representation with supplied format
const char* DissolveSys::ftoa(double f, const char* fmt)
{
	static CharString result;
	result.sprintf(fmt,f);
	return result;
}

// Convert string to boolean
bool DissolveSys::atob(const char* s)
{
	if (sameString(s, "true") || sameString(s, "on")) return true;
	return false;
}

// Convert boolean to string representation ("True" or "False")
const char* DissolveSys::btoa(bool b)
{
	return (b ? "True" : "False");
}

// Convert boolean to string representation ("On" or "Off")
const char* DissolveSys::onOff(bool b)
{
	return (b ? "On" : "Off");
}

/*
 * String Functions
 */

// Convert string to uppercase
const char* DissolveSys::upperCase(const char* s)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++) result += toupper(s[i]);
	return result;
}

// Convert string to lowercase
const char* DissolveSys::lowerCase(const char* s)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++) result += tolower(s[i]);
	return result;
}

// Perform case-(in)sensitive string comparison
bool DissolveSys::sameString(const char* s1, const char* s2, bool caseSensitive)
{
	int len1 = strlen(s1), len2 = strlen(s2);
	if (len1 != len2) return false;

	if (caseSensitive)
	{
		for (int n=0; n<len1; ++n) if (s1[n] != s2[n]) return false;
	}
	else
	{
		for (int n=0; n<len1; ++n) if (tolower(s1[n]) != tolower(s2[n])) return false;
	}

	return true;
}

// Perform case-(in)sensitive, wildcard-enabled string comparison
bool DissolveSys::sameWildString(const char* wild, const char* s2, bool caseSensitive)
{
	// If we reach at the end of both strings, the match was a success
	if ((*wild == '\0') && (*s2 == '\0')) return true;

	// Handle wildcards '*' and '?'
	if (*wild == '*')
	{
		/*
		 * If this is not the last character in the string and there are no more characters in s2, return false.
		 * Otherwise, we try to match the next character in 'wild' from the current character in s2, then the current character in wild with the next character in s2.
		 * This allows the '*' to match as many characters as it can, but always checks for absolute character matching between the current characters.
		 */
		if ((*(wild+1) != '\0') && (*s2 == '\0')) return false;
		else return sameWildString(wild+1, s2, caseSensitive) || sameWildString(wild, s2+1, caseSensitive);
	}
	else
	{
		// Wildcard '?' matches any single character, so increase both character positions and continue
		if (*wild == '?') return sameWildString(wild+1, s2+1, caseSensitive);
	}

	// Not a wildcard, so do absolute character matching
	if (caseSensitive)
	{
		if (tolower(*wild) == tolower(*s2)) return sameWildString(wild+1, s2+1, caseSensitive);
	}
	else if (*wild == *s2) return sameWildString(wild+1, s2+1, caseSensitive);

	// Characters do not match, so return false
	return false;
}

// Get characters before first occurrence of designated character, or an empty string if the character does not exist
const char* DissolveSys::beforeChar(const char* s, char delim)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (s[i] == delim) return result;
		result += s[i];
	}
	return "";
}

// Get characters after first occurrence of designated character, or an empty string if the character does not exist
const char* DissolveSys::afterChar(const char* s, char delim)
{
	static CharString result(1024);
	result.clear();
	bool found = false;
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (found) result += s[i];
		if (s[i] == delim) found = true;
	}
	return (found ? result.get() : "");
}

// Get characters after last occurrence of designated character
const char* DissolveSys::afterLastChar(const char* s, char delim)
{
	static CharString result;
	result.clear();
	const char* c, *d = NULL;
	for (c = &s[0]; *c != '\0'; ++c) if (*c == delim) d = c;
	if (d != NULL) result = ++d;
	return result;
}

// Get characters before last occurrence of designated character
const char* DissolveSys::beforeLastChar(const char* s, char delim)
{
	static CharString result;
	result.clear();
	
	// First, find last occurrence of specified character
	int lastPos = 0, count = 0;
	const char* c;
	for (c = &s[0]; *c != '\0'; ++c, ++count) if (*c == delim) lastPos = count;
	
	// Now, grab 'lastPos' characters from the string
	for (count = 0; count < lastPos; ++count) result += s[count];
	return result;
}

// Get characters before first occurrence of designated string
const char* DissolveSys::beforeStr(const char* s, const char* search)
{
	// Search for first occurrence of string
	static char result[8096];
	strcpy(result, s);
	char* c = strstr(result, search);
	if (c == NULL) return "";
	*c = '\0';
	return result;
}

// Get characters after first occurrence of designated character
const char* DissolveSys::afterStr(const char* s, const char* search)
{
	const char* c = strstr(s, search);
	if (c == NULL) return "";
	for (const char* d = &search[0]; *d != '\0'; ++d) c++;
	return c;
}

// Return if the target string ends with the specified characters
bool DissolveSys::endsWith(const char* target, const char* ending)
{
	// Check for enough characters to compare
	const int targetLength = strlen(target);
	const int endingLength = strlen(ending);
	if (targetLength < endingLength) return false;

	const char* c1 = &target[targetLength-1];
	const char* c2 = &ending[endingLength-1];
	for (int n=0; n<endingLength; ++n)
	{
		if ((*c1) != (*c2)) return false;
		--c1;
		--c2;
	}

	return true;
}

// Remove comments from line
void DissolveSys::removeComments(char* s)
{
	char* c, quotechar = '\0';
	bool escaped = false;
	for (c = s; *c != '\0'; ++c)
	{
		// Remember current quoting info...
		if (*c == '"')
		{
			if (quotechar == '\0') quotechar = '"';
			else if (quotechar == '"') quotechar = '\0';
		}
		if (*c == '\'')
		{
			if (quotechar == '\0') quotechar = '\'';
			else if (quotechar == '\'') quotechar = '\0';
		}
		if ((*c == '#') && (!escaped) && (quotechar == '\0'))
		{
			*c = '\0';
			break;
		}
		// Removed July 2019 - C-style comments not permitted in input files as this breaks object tags (which contain '//' as delimiters)
// 		else if ((*c == '/') && (!escaped) && (quotechar == '\0'))
// 		{
// 			char* c2 = c;
// 			c2++;
// 			if (*c2 == '/')
// 			{
// 				*c = '\0';
// 				break;
// 			}
// 		}
		escaped = *c == '\\';
	}
}

// Strip trailing whitespace from string
const char* DissolveSys::stripTrailing(const char* s)
{
	int n;
	static char result[1024];
	// Go backwards through string and find first non-whitespace character
	for (n=strlen(s)-1; n>=0; n--) if (s[n] != ' ') break;
	strncpy(result,s,n+1);
	result[n+1] = '\0';
	return result;
}

// Replace all of the supplied characters in the source string
const char* DissolveSys::replaceChars(const char* s, const char* charstoreplace, char r)
{
	static CharString result;
	bool found;
	char const* c1, *c2;
	result.clear();
	for (c1 = &s[0]; *c1 != '\0'; c1++)
	{
		found = false;
		for (c2 = charstoreplace; *c2 != '\0'; c2++)
		{
			if (*c1 == *c2)
			{
				found = true;
				break;
			}
		}
		if (found) result += r;
		else result += *c1;
	}
	return result.get();
}

// Strip all of the supplied characters from the source string
const char* DissolveSys::stripChars(const char* s, const char* charstostrip)
{
	static CharString result;
	char const* c1, *c2;
	bool found;
	result.clear();
	for (c1 = &s[0]; *c1 != '\0'; c1++)
	{
		found = false;
		for (c2 = &charstostrip[0]; *c2 != '\0'; c2++)
		{
			if (*c1 == *c2)
			{
				found = true;
				break;
			}
		}
		if (!found) result += *c1;
	}
	return result.get();
}

// Count number of times that supplied characters occur in supplied string
int DissolveSys::countChars(const char* s, const char* chars, int offset)
{
	int total = 0, n, count = 0;
	while (*s != '\0')
	{
		if (count >= offset)
		{
			for (n=0; chars[n] != '\0'; n++) if (chars[n] == *s) total ++;
		}
		s++;
		count++;
	}
	return total;
}

// Return whether string consists of empty whitespace characters only
bool DissolveSys::isEmpty(const char* s)
{
	if (s == NULL) return true;
	for (const char* c = s; *c != '\0'; ++c)
	{
		switch (*c)
		{
			case (' '):
			case ('\t'):
			case ('\n'):
			case ('\r'):
				continue;
			default:
				return false;
		}
	}
	return true;
}

// Return nice version of string (no spaces, slashes etc.)
const char* DissolveSys::niceName(const char* original)
{
	static CharString result;
	result = original;
	result.replace(" /\\#*$", '_');

	return result.get();
}


/*
 * Number Detection
 */

// Return whether the supplied string is a number
bool DissolveSys::isNumber(const char* text)
{
	bool isFloatingPoint;
	return isNumber(text, isFloatingPoint);
}

// Return whether the supplied string is a number, and also whether it is floating-point
bool DissolveSys::isNumber(const char* text, bool& isFloatingPoint)
{
	// Assume integer to start with
	isFloatingPoint = false;

	int exponentIndex = -1;

	const int length = strlen(text);
	for (int n=0; n<length; ++n)
	{
		char c = text[n];
		switch (text[n])
		{
			// Decimal point
			case ('.'):
				isFloatingPoint = true;
				break;
			// Negate or plus
			case ('-'):
			case ('+'):
				// Only allow as first character or immediately following an exponent
				if (n != (exponentIndex+1)) return false;
				break;
			// Exponentiation
			case ('e'):
			case ('E'):
				// Can't be first character
				if (n == 0) return false;

				// Can't have more than one
				if (exponentIndex > 0) return false;

				// Store position
				exponentIndex = n;
				break;
			default:
				// If not a digit, return false
				if (!isdigit(c)) return false;
				break;
		}
	}

	return true;
}

/*
 * Files
 */

// Return whether file exists
bool DissolveSys::fileExists(const char* filename)
{
	fstream f(filename,ios::in);
	if (f.is_open())
	{
		f.close();
		return true;
	}
	else return false;
}

/*
 * Time Functions
 */

// Return string of current time / date
const char* DissolveSys::currentTimeAndDate()
{
	static char result[128];
	time_t currentTime = time(NULL);
	tm* converted = localtime(&currentTime);
	strftime(result, sizeof(result), "%H:%M:%S on %d-%m-%Y", converted);
	return result;
}
