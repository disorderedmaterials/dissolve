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

// Register modules
void DUQ::registerModules()
{
	// Manually register all modules here (annoying, but easier than trying to work out a self-initialising class that doesn't get gazumped by the linker removing all references to things we want...)
	// -- Analysis Modules
	// modules_[Module::AnalysisModule].own(new XXX);
	// -- Evolution Modules
	modules_[Module::EvolutionModule].own(new AtomShake);

	Messenger::printVerbose("\n");
	Messenger::printVerbose("Module Information:\n\n");

	// Print out module information
	for (int n=0; n<Module::nModuleTypes; ++n)
	{
		Module::ModuleType mt = (Module::ModuleType) n;

		// Print header
		Messenger::printVerbose("%s Modules (%i):\n", Module::moduleType(mt), modules_[mt].nItems());

		if (modules_[mt].nItems() == 0) Messenger::printVerbose(" --> No modules of this type registered.\n");
		else for (Module* module = modules_[mt].first(); module != NULL; module = module->next)
		{
			Messenger::printVerbose(" --> %s\n", module->name());
			Messenger::printVerbose("     %s\n", module->brief());
		}

		Messenger::printVerbose("\n");
	}
}
