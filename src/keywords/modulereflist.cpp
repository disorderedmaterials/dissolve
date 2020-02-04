/*
	*** Keyword - Module RefList
	*** src/keywords/modulereflist.cpp
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

#include "keywords/modulereflist.h"
#include "module/list.h"
#include "module/module.h"
#include "classes/coredata.h"
#include "base/lineparser.h"

// Constructors
ModuleRefListKeyword::ModuleRefListKeyword(RefList<Module>& references, int maxModules) : KeywordData< RefList<Module>& >(KeywordBase::ModuleRefListData, references)
{
	maxModules_ = maxModules;
}

ModuleRefListKeyword::ModuleRefListKeyword(RefList<Module>& references, CharStringList allowedModuleTypes, int maxModules) : KeywordData< RefList<Module>& >(KeywordBase::ModuleRefListData, references)
{
	moduleTypes_ = allowedModuleTypes;
	maxModules_ = maxModules;
}

// Destructor
ModuleRefListKeyword::~ModuleRefListKeyword()
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool ModuleRefListKeyword::isDataEmpty() const
{
	return data_.nItems() > 0;
}

// Return the Module type(s) to allow
const CharStringList& ModuleRefListKeyword::moduleTypes() const
{
	return moduleTypes_;
}

// Return maximum number of Modules to allow in the list
int ModuleRefListKeyword::maxModules() const
{
	return maxModules_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ModuleRefListKeyword::minArguments() const
{
	return 1;
}

// Return maximum number of arguments accepted
int ModuleRefListKeyword::maxArguments() const
{
	return (maxModules_ == -1 ? 99 : maxModules_);
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool ModuleRefListKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	// Loop over arguments provided to the keyword
	for (int n = startArg; n<parser.nArgs(); ++n)
	{
		// Find specified Module by its unique name
		Module* module = coreData.findModule(parser.argc(n));
		if (!module)
		{
			Messenger::error("No Module named '%s' exists.\n", parser.argc(n));
			return false;
		}

		// Check the module's type
		if ((moduleTypes_.nItems() > 0) && (!moduleTypes_.contains(module->type())))
		{
			Messenger::error("Module '%s' is of type '%s', and is not permitted in this list (allowed types = %s).\n", parser.argc(n), module->type(), moduleTypes_.asCommaSeparatedList());
			return false;
		}

		data_.append(module);
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool ModuleRefListKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	// Loop over list of referenced Modules
	for (Module* module : data_)
	{
		if (!parser.writeLineF("%s%s  '%s'\n", prefix, keywordName, module->uniqueName())) return false;
	}

	return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Module in the contained data
void ModuleRefListKeyword::removeReferencesTo(Module* module)
{
	data_.remove(module);
}
