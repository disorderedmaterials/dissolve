/*
	*** Module Keyword - Module Reference List
	*** src/modules/keywordtypes/modulereferencelist.cpp
	Copyright T. Youngs 2012-2018

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

#include "module/keywordtypes/modulereferencelist.h"
#include "module/list.h"
#include "module/module.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
ModuleReferenceListModuleKeyword::ModuleReferenceListModuleKeyword(RefList<Module,bool>& references, const char* moduleType) : ModuleKeywordBase(ModuleKeywordBase::ModuleReferenceListData), ModuleKeywordData< RefList<Module,bool>& >(references)
{
	moduleType_ = moduleType;
}

// Destructor
ModuleReferenceListModuleKeyword::~ModuleReferenceListModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool ModuleReferenceListModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ModuleReferenceListModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int ModuleReferenceListModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool ModuleReferenceListModuleKeyword::read(LineParser& parser, int startArg, ProcessPool& procPool)
{
	// Find specified Module by its unique name
	Module* module = ModuleList::findInstanceByUniqueName(parser.argc(startArg));
	if (!module)
	{
		Messenger::error("No Module named '%s' exists.\n", parser.argc(startArg));
		return false;
	}
	if (!DissolveSys::sameString(module->name(), moduleType_))
	{
		Messenger::error("Module '%s' is not of the correct type (%s).\n", parser.argc(startArg), module->name());
		return false;
	}

	data_.add(module);

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool ModuleReferenceListModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of IsotopologueReferences
	RefListIterator<Module,bool> refIterator(data_);
	while (Module* module = refIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), module->uniqueName())) return false;
	}

	return true;
}
