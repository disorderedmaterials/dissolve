/*
	*** dUQ Modules
	*** src/main/modules.cpp
	Copyright T. Youngs 2012-2016

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

#include "main/duq.h"
#include "modules/atomshake.h"
#include "base/sysfunc.h"

// Register modules
bool DUQ::registerModules()
{
	// Manually register all modules here (annoying, but easier than trying to work out a self-initialising class that doesn't get gazumped by the linker removing all references to things we want...)
	// -- Analysis Modules
	// modules_[Module::AnalysisModule].own(new XXX);
	// -- Evolution Modules
	modules_[Module::EvolutionModule].add(new AtomShake);

	// Construct master list of modules, and do sanity check on names
	for (int n=0; n<Module::nModuleTypes; ++n)
	{
		Module::ModuleType mt = (Module::ModuleType) n;
		for (RefListItem<Module,bool>* newItem = modules_[mt].first(); newItem != NULL; newItem = newItem->next)
		{
			Module* module = newItem->item;

			// Loop over items already in the allModules_ reflist, checking names against this one
			for (RefListItem<Module,bool>* existingItem = allModules_.first(); existingItem != NULL; existingItem = existingItem->next)
			{
				if (DUQSys::sameString(existingItem->item->name(), module->name()))
				{
					Messenger::error("Two modules cannot have the same name.\n");
					return false;
				}
			}

			// OK, so add to the list
			allModules_.add(module);
		}
	}

	Messenger::printVerbose("\n");
	Messenger::printVerbose("Module Information:\n\n");

	// Print out module information
	for (int n=0; n<Module::nModuleTypes; ++n)
	{
		Module::ModuleType mt = (Module::ModuleType) n;

		// Print header
		Messenger::printVerbose("%s Modules (%i):\n", Module::moduleType(mt), modules_[mt].nItems());

		if (modules_[mt].nItems() == 0) Messenger::printVerbose(" --> No modules of this type registered.\n");
		else for (RefListItem<Module,bool>* existingItem = allModules_.first(); existingItem != NULL; existingItem = existingItem->next)
		{
			Module* module = existingItem->item;

			Messenger::printVerbose(" --> %s\n", module->name());
			Messenger::printVerbose("     %s\n", module->brief());
		}

		Messenger::printVerbose("\n");
	}

	return true;
}

// Find Module by name
Module* DUQ::findModule(const char* name)
{
	for (RefListItem<Module,bool>* ri = allModules_.first(); ri != NULL; ri = ri->next) if (DUQSys::sameString(name, ri->item->name())) return ri->item;

	return NULL;
}
