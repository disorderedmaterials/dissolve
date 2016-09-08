/*
	*** System functions
	*** src/base/sysfunc.h
	Copyright T. Youngs 2012-2014

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

// Number/string conversion
const char* itoa(int);
const char* ftoa(double);
const char* ftoa(double, const char*);

// String functions
const char* upperCase(const char*);
const char* lowerCase(const char*);
const char* beforeChar(const char*, char);
const char* afterChar(const char*, char);
const char* afterLastChar(const char*, char);
const char* beforeLastChar(const char*, char);
const char* beforeStr(const char*, const char*);
const char* afterStr(const char*, const char*);
const char* stripTrailing(const char*);
const char* replaceChars(const char* s, const char* charstoreplace, char r);
const char* stripChars(const char* s, const char* charstostrip);
int countChars(const char* string, const char* searchchars, int offset = 0);
void removeComments(char* s);
bool isEmpty(const char* s);

// Enum search and print
int enumSearch(const char* name, int nitems, const char**list, const char* query, bool reportError = 1);
void enumPrintValid(int nitems, const char**list);

// Files
bool fileExists(const char* filename);

// Responses
bool getBooleanResponse(const char* promptText, bool hasDefaultValue, bool defaultValue);

#endif
