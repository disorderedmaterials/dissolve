/*
	*** Module Keyword Group
	*** src/module/keywordgroup.h
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

#ifndef DISSOLVE_KEYWORDGROUP_H
#define DISSOLVE_KEYWORDGROUP_H

#include "base/charstring.h"
#include "templates/reflist.h"
#include "templates/listitem.h"

// Forward Declarations
class ModuleKeywordBase;
class ModuleKeywordList;

// Module Keyword Group
class ModuleKeywordGroup : public ListItem<ModuleKeywordGroup>
{
	public:
	// Constructor
	ModuleKeywordGroup(ModuleKeywordList& keywordList);


	/*
	 * Identity
	 */
	private:
	// Name of the group
	CharString name_;

	public:
	// Set name of group
	void setName(const char* name);
	// Return name of group
	const char* name() const;


	/*
	 * Keyword Group
	 */
	private:
	// Associated ModuleKeywordList
	ModuleKeywordList& keywordList_;
	// List of keywords (in the referenced ModuleKeywordList) that are in this group
	RefList<ModuleKeywordBase,bool> keywords_;

	private:
	// Add specified keyword to the group
	void addKeywordToGroup(ModuleKeywordBase* object);

	public:
	// Add keyword (pass-thru to ModuleKeywordList)
	bool add(ModuleKeywordBase* object, const char* keyword, const char* description, int genericItemFlags = 0);
	// Add keyword (pass-thru to ModuleKeywordList)
	bool add(ModuleKeywordBase* object, const char* keyword, const char* description, const char* arguments, int genericItemFlags = 0);
	// Return reference list of keywords in group
	RefList<ModuleKeywordBase,bool>& keywords();
};

#endif

