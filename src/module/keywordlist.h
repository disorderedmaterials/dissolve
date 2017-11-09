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
#include "module/keyworddata.h"
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
	// Return simple keyword value (as bool)
	bool asBool(const char* keywordName);
	// Return simple keyword value (as int)
	int asInt(const char* keywordName);
	// Return simple keyword value (as double)
	double asDouble(const char* keywordName);
	// Return simple keyword value (as string)
	const char* asString(const char* keywordName);
	// Return whether the specified keyword data has ever been set
	bool set(const char* keywordName);
};

// Keyword List Helper
template <class T> class KeywordListHelper
{
	public:
	// Retrieve named item from specified list as template-guided type
	static T& retrieve(ModuleKeywordList& sourceList, const char* name, T defaultValue = T(), bool* found = NULL)
	{
		// Find item in the list
		ModuleKeywordBase* item = sourceList.find(name);
		if (!item)
		{
			Messenger::printVerbose("No item named '%s' in the keyword list - default value item will be returned.\n", name);
			static T dummy;
			dummy = defaultValue;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		// Attempt to cast to interface
		ModuleKeywordData<T>* castItem = dynamic_cast<ModuleKeywordData<T>*>(item);
		if (!castItem)
		{
			printf("That didn't work, because it's of the wrong type.\n");
			static T dummy;
			if (found != NULL) (*found) = false;
			return dummy;
		}

		if (found != NULL) (*found) = true;
		return castItem->data();
	}
};

#endif

