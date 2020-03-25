/*
	*** Export Trajectory Module - Processing
	*** src/modules/export_trajectory/process.cpp
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

#include "modules/export_trajectory/exporttraj.h"
#include "main/dissolve.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Run main processing
bool ExportTrajectoryModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	if (!trajectoryFormat_.hasValidFileAndFormat()) Messenger::error("No valid file/format set for trajectory export.\n");

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0) return Messenger::error("No configuration target set for module '%s'.\n", uniqueName());

	// Loop over target Configurations
	auto* cfg = targetConfigurations_.firstItem();

	// Set up process pool - must do this to ensure we are using all available processes
	procPool.assignProcessesToGroups(cfg->processPool());

	// Only the pool master saves the data
	if (procPool.isMaster())
	{
		Messenger::print("Export: Appending trajectory file (%s) for Configuration '%s'...\n", trajectoryFormat_.description(), cfg->name());

		if (!trajectoryFormat_.exportData(cfg))
		{
			Messenger::print("Export: Failed to append trajectory file '%s'.\n", trajectoryFormat_.filename());
			procPool.decideFalse();
			return false;
		}

		procPool.decideTrue();
	}
	else if (!procPool.decision()) return false;

	return true;
}
