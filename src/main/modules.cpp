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
#include "modules/energy.h"
#include "modules/export.h"
#include "modules/fq.h"
#include "modules/partials.h"
#include "base/sysfunc.h"

// Register modules
bool DUQ::registerModules()
{
	// Manually register all modules here (annoying, but easier than trying to work out a self-initialising class that doesn't get gazumped by the linker removing all references to things we want...)
	modules_.add(new AtomShake);
	modules_.add(new Energy);
	modules_.add(new Export);
	modules_.add(new Partials);
	modules_.add(new StructureFactor);

	// Construct master list of modules, and do sanity check on names
	for (RefListItem<Module,bool>* ri = modules_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Module pointer
		Module* module = ri->item;

		// Loop over items already in the allModules_ reflist, checking names against this one
		for (RefListItem<Module,bool>* rj = ri->next; rj != NULL; rj = rj->next)
		{
			if (DUQSys::sameString(rj->item->name(), module->name()))
			{
				Messenger::error("Two modules cannot have the same name (%s).\n", module->name());
				return false;
			}
		}
	}

	// Print out module information
	Messenger::printVerbose("\n");
	Messenger::printVerbose("Module Information (%i available):\n\n", modules_.nItems());
	for (RefListItem<Module,bool>* existingItem = modules_.first(); existingItem != NULL; existingItem = existingItem->next)
	{
		Module* module = existingItem->item;
		Messenger::printVerbose(" --> %s\n", module->name());
		Messenger::printVerbose("     %s\n", module->brief());
	}

	return true;
}

// Find Module by name
Module* DUQ::findModule(const char* name)
{
	for (RefListItem<Module,bool>* ri = modules_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Module pointer
		Module* module = ri->item;
		
		if (DUQSys::sameString(name, ri->item->name())) return ri->item;
	}

	return NULL;
}
