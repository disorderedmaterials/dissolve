/*
	*** Module Keyword List
	*** src/module/keywordlist.cpp
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

#include "module/keywordlist.h"
#include "base/sysfunc.h"

// Add keyword
bool ModuleKeywordList::add(ModuleKeywordBase* object, const char* keyword, const char* description, int genericItemFlags)
{
	return add(object, keyword, description, "", genericItemFlags);
}

// Add keyword
bool ModuleKeywordList::add(ModuleKeywordBase* object, const char* keyword, const char* description, const char* arguments, int genericItemFlags)
{
	// Take ownership of the passed object, and set its basic information
	keywords_.own(object);
	object->set(keyword, description, arguments, genericItemFlags);

	return true;
}

// Find named keyword
ModuleKeywordBase* ModuleKeywordList::find(const char* keyword)
{
	for (ModuleKeywordBase* kwd = keywords_.first(); kwd != NULL; kwd = kwd->next) if (DUQSys::sameString(keyword, kwd->keyword())) return kwd;

	return NULL;
}

/*
 * Conversion
 */

// Return simple default value (as bool)
bool ModuleKeywordList::asBool(const char* keywordName)
{
	// Find the named keyword
	ModuleKeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists top return as a bool. Returning 'false'...\n", keywordName);
		return false;
	}

	return keyword->asBool();
}

// Return simple default value (as int)
int ModuleKeywordList::asInt(const char* keywordName)
{
	// Find the named keyword
	ModuleKeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as an int. Returning '0'...\n", keywordName);
		return 0;
	}

	return keyword->asInt();
}

// Return simple default value (as double)
double ModuleKeywordList::asDouble(const char* keywordName)
{
	// Find the named keyword
	ModuleKeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a double. Returning '0.0'...\n", keywordName);
		return 0.0;
	}

	return keyword->asDouble();
}

// Return simple default value (as string)
const char* ModuleKeywordList::asString(const char* keywordName)
{
	// Find the named keyword
	ModuleKeywordBase* keyword = find(keywordName);
	if (!keyword)
	{
		Messenger::warn("No Module keyword named '%s' exists to return as a string. Returning 'NULL'...\n", keywordName);
		return "NULL";
	}

	return keyword->asString();
}
