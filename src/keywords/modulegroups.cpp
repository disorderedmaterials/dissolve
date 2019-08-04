/*
	*** Module Keyword - Module Groups
	*** src/modules/keywordtypes/modulegroups.cpp
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

#include "keywords/modulegroups.h"
#include "module/group.h"
#include "module/groups.h"
#include "module/module.h"
#include "classes/coredata.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
ModuleGroupsModuleKeyword::ModuleGroupsModuleKeyword(ModuleGroups& groups) : ModuleKeywordData<ModuleGroups&>(ModuleKeywordBase::ModuleGroupsData, groups)
{
}

// Destructor
ModuleGroupsModuleKeyword::~ModuleGroupsModuleKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ModuleGroupsModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int ModuleGroupsModuleKeyword::maxArguments()
{
	// Module name plus group name
	return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool ModuleGroupsModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	// Find specified Module by its unique name
	Module* module = coreData.findModule(parser.argc(startArg));
	if (!module)
	{
		Messenger::error("No Module named '%s' exists.\n", parser.argc(startArg));
		return false;
	}

	// Check the module's type
	if (!data_.moduleTypeIsAllowed(module->type()))
	{
		Messenger::error("Module '%s' is of type '%s', and is not permitted in these groups (allowed types = %s).\n", parser.argc(startArg), module->type(), data_.allowedModuleTypes().asCommaSeparatedList());
		return false;
	}

	// If a second argument was given, this is the name of the group we should add the Module to. Otherwise, use the default
	data_.addModule(module, parser.hasArg(startArg+1) ? parser.argc(startArg+1) : "Default");

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool ModuleGroupsModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over defined groups
	ListIterator<ModuleGroup> groupIterator(data_.groups());
	while (ModuleGroup* group = groupIterator.iterate())
	{
		// Loop over list of referenced Modules in this group
		RefListIterator<Module> refIterator(group->modules());
		while (Module* module = refIterator.iterate())
		{
			if (!parser.writeLineF("%s%s  '%s'  '%s'\n", prefix, keyword(), module->uniqueName(), group->name())) return false;
		}
	}

	return true;
}
