/*
	*** System Functions
	*** src/base/sysfunc.cpp
	Copyright T. Youngs 2012-2017

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

#include "base/sysfunc.h"
#include "base/messenger.h"
#include <fstream>
#include <iostream>
#include <string.h>

using namespace std;

/*
 * Conversion Functions
 */

// Convert integer to string representation
const char* DUQSys::itoa(int n)
{
	static CharString result;
	result.sprintf("%i",n);
	return result;
}

// Convert double to string representation (as %f)
const char* DUQSys::ftoa(double f)
{
	static CharString result;
	result.sprintf("%f",f);
	return result;
}

// Convert double to string representation with supplied format
const char* DUQSys::ftoa(double f, const char* fmt)
{
	static CharString result;
	result.sprintf(fmt,f);
	return result;
}

// Convert string to boolean
bool DUQSys::atob(const char* s)
{
	if (sameString(s, "true") || sameString(s, "on")) return true;
	return false;
}

// Convert boolean to string representation ("True" or "False")
const char* DUQSys::btoa(bool b)
{
	return (b ? "True" : "False");
}

// Convert boolean to string representation ("On" or "Off")
const char* DUQSys::onOff(bool b)
{
	return (b ? "On" : "Off");
}

/*
 * String Functions
 */

// Convert string to uppercase
const char* DUQSys::upperCase(const char* s)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++) result += toupper(s[i]);
	return result;
}

// Convert string to lowercase
const char* DUQSys::lowerCase(const char* s)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++) result += tolower(s[i]);
	return result;
}

// Perform case-insensitive string comparison
bool DUQSys::sameString(const char* s1, const char* s2, bool caseSensitive)
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

// Get characters before first occurrence of designated character
const char* DUQSys::beforeChar(const char* s, char delim)
{
	static CharString result(1024);
	result.clear();
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (s[i] == delim) break;
		result += s[i];
	}
	return result;
}

// Get characters after first occurrence of designated character
const char* DUQSys::afterChar(const char* s, char delim)
{
	static CharString result(1024);
	result.clear();
	bool found = false;
	for (int i = 0; s[i] != '\0'; i++)
	{
		if (found) result += s[i];
		if (s[i] == delim) found = true;
	}
	return result;
}

// Get characters after last occurrence of designated character
const char* DUQSys::afterLastChar(const char* s, char delim)
{
	static CharString result;
	result.clear();
	const char* c, *d = NULL;
	for (c = &s[0]; *c != '\0'; ++c) if (*c == delim) d = c;
	if (d != NULL) result = ++d;
	return result;
}

// Get characters before last occurrence of designated character
const char* DUQSys::beforeLastChar(const char* s, char delim)
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
const char* DUQSys::beforeStr(const char* s, const char* search)
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
const char* DUQSys::afterStr(const char* s, const char* search)
{
	const char* c = strstr(s, search);
	if (c == NULL) return "";
	for (const char* d = &search[0]; *d != '\0'; ++d) c++;
	return c;
}

// Remove comments from line
void DUQSys::removeComments(char* s)
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
		else if ((*c == '/') && (!escaped) && (quotechar == '\0'))
		{
			char* c2 = c;
			c2++;
			if (*c2 == '/')
			{
				*c = '\0';
				break;
			}
		}
		escaped = *c == '\\';
	}
}

// Strip trailing whitespace from string
const char* DUQSys::stripTrailing(const char* s)
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
const char* DUQSys::replaceChars(const char* s, const char* charstoreplace, char r)
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
const char* DUQSys::stripChars(const char* s, const char* charstostrip)
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
int DUQSys::countChars(const char* s, const char* chars, int offset)
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
bool DUQSys::isEmpty(const char* s)
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

/*
 * Files
 */

// Return whether file exists
bool DUQSys::fileExists(const char* filename)
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
const char* DUQSys::currentTimeAndDate()
{
	static char result[128];
	time_t currentTime = time(NULL);
	tm* converted = gmtime(&currentTime);
	strftime(result, sizeof(result), "%H:%M:%S on %d-%m-%Y", converted);
	return result;
}
