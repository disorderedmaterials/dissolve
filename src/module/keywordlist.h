/*
	*** Module Keyword List
	*** src/module/keywordlist.h
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

#ifndef DUQ_KEYWORDLIST_H
#define DUQ_KEYWORDLIST_H

#include "module/keywordbase.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Module Keyword List
class ModuleKeywordList
{
	/*
	 * Keyword List
	 */
	private:
	// List of defined keywords
	List<ModuleKeywordBase> keywords_;

	public:
	// Add keyword
	bool add(ModuleKeywordBase* object, const char* keyword, const char* description, int genericItemFlags = 0);
	// Add keyword
	bool add(ModuleKeywordBase* object, const char* keyword, const char* description, const char* arguments, int genericItemFlags = 0);
	// Find named keyword
	ModuleKeywordBase* find(const char* keyword);
	// Return first keyword in list
	ModuleKeywordBase* keywords();


	/*
	 * Conversion
	 */
	public:
	// Return simple default value (as bool)
	bool asBool(const char* keywordName);
	// Return simple default value (as int)
	int asInt(const char* keywordName);
	// Return simple default value (as double)
	double asDouble(const char* keywordName);
	// Return simple default value (as string)
	const char* asString(const char* keywordName);
};

#endif

