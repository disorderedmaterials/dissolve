/*
	*** EPSR Module - Functions
	*** src/modules/epsr/functions.cpp
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

#include "main/dissolve.h"
#include "modules/epsr/epsr.h"
#include "classes/atomtype.h"
#include "classes/masterintra.h"
#include "classes/scatteringmatrix.h"
#include "classes/speciesbond.h"
#include "classes/partialset.h"
#include "math/praxis.h"
#include "templates/genericlisthelper.h"

// Add Module target to specified group
bool EPSRModule::addTarget(const char* moduleTarget, const char* group)
{
	// First, find the named Module
	Module* module = ModuleList::findInstanceByUniqueName(moduleTarget);
	if (!module) return Messenger::error("Couldn't find Module '%s' to add to EPSRModule's list of targets.\n", moduleTarget);

	// Check on the type of the Module given... if OK, add to the specified group
	if (DissolveSys::sameString(module->name(), "NeutronSQ")) Messenger::print("Adding NeutronSQ target '%s' to '%s'.\n", moduleTarget, uniqueName());
	else return Messenger::error("Can't use Module of type '%s' as a fitting target.\n", module->name());

	// Does the specified group exist?
	ModuleGroup* moduleGroup;
	for (moduleGroup = targetGroups_.first(); moduleGroup != NULL; moduleGroup = moduleGroup->next) if (DissolveSys::sameString(moduleGroup->name(), group)) break;
	if (moduleGroup == NULL)
	{
		moduleGroup = new ModuleGroup(group);
		targetGroups_.own(moduleGroup);
	}

	targets_.add(module);
	moduleGroup->add(module);

	return true;
}

// Return list of target Modules / data for fitting process
const RefList<Module,bool>& EPSRModule::targets() const
{
	return targets_;
}

// Return list of target groups defined
const List<ModuleGroup>& EPSRModule::targetGroups() const
{
	return targetGroups_;
}
