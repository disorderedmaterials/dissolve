/*
	*** Skeleton Module - Method
	*** src/modules/skeleton/method.cpp
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

#include "modules/skeleton/skeleton.h"
#include "main/duq.h"
#include "base/sysfunc.h"

// Perform set up tasks for module
bool SkeletonModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool SkeletonModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool SkeletonModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * This is a XXX routine.
	 * XXX
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Setup process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// MODULE CODE
	}

	return false;
}

// Execute post-processing stage
bool SkeletonModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}
