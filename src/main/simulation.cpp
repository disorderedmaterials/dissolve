/*
	*** dUQ - Simulation
	*** src/main/simulation.cpp
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
#include "base/sysfunc.h"

// Find first occurrence of named Module in pre-processing tasks
Module* DUQ::findPreProcessingTask(const char* name)
{
	for (RefListItem<Module,bool>* ri = preProcessingTasks_.first(); ri != NULL; ri = ri->next)
	{
		Module* module = ri->item;

		if (DUQSys::sameString(name, module->name())) return module;
	}

	return NULL;
}

// Find first occurrence of named Module in post-processing tasks
Module* DUQ::findPostProcessingTask(const char* name)
{
	for (RefListItem<Module,bool>* ri = postProcessingTasks_.first(); ri != NULL; ri = ri->next)
	{
		Module* module = ri->item;

		if (DUQSys::sameString(name, module->name())) return module;
	}

	return NULL;
}

// Run Simulation
bool DUQ::go()
{
	/*
	 * This is the main program loop. We perform the following steps:
	 *
	 *  1)	Run all Module pre-processing tasks using all processes available (worldPool_)
	 *  2)	Run all Modules assigned to Configurations using individual parallel strategies for Configurations
	 *  3)	Reassemble Configuration data on all processes
	 *  4)	Run all Modules assigned to Samples using all processes available (worldPool_)
	 *  5)	Run any Module post-processing tasks using all processes available (worldPool_)
	 */

	/*
	 * Start of Main Loop
	 */
	int iteration = 0;
	bool keepGoing = true;

	do
	{
		// Increase iteration counter
		++iteration;

		Messenger::print("\n");
		Messenger::print("===============================================\n");
		Messenger::print("  MAIN LOOP ITERATION %10i / %-10s\n", iteration, maxIterations_ == -1 ? "(no limit)" : DUQSys::itoa(maxIterations_));
		Messenger::print("===============================================\n");
		Messenger::print("\n");

		/*
		 *  1) 	Perform pre-processing tasks (using worldPool_)
		 */
		Messenger::print("\n");
		Messenger::print("===== Pre-Processing\n");
		for (RefListItem<Module,bool>* ri = preProcessingTasks_.first(); ri != NULL; ri = ri->next)
		{
			// Grab Module pointer
			Module* module = ri->item;

			Messenger::print("\n");
			Messenger::print("--> Module '%s'\n", module->name());

			// Execute the pre-processing stage
			if (!module->preProcess(*this, worldPool_))
			{
				Messenger::error("Module '%s' experienced errors. Exiting now.\n", module->name());
				return false;
			}
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		/*
		 *  2)	Loop over Configurations, running their modules in the sequence they are defined
		 * 	If a process is not involved in the Configuration's ProcessPool, it can move on
		 */
		Messenger::print("\n");
		Messenger::print("===== Configuration Processing\n");

		bool result = true;
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			// Check for failure of one or more processes / processing tasks
			if (!worldPool_.ok(result))
			{
				Messenger::error("One or more processes experienced failures. Exiting now.\n");
				return false;
			}

			Messenger::print("\n");
			Messenger::print("--> Configuration '%s'\n", cfg->name());

			// Check involvement of this process
			if (!cfg->processPool().involvesMe())
			{
				Messenger::print("Process rank %i not involved with this Configuration, so moving on...\n", ProcessPool::worldRank());
				continue;
			}

			// Loop over Modules defined in the Configuration
			for (RefListItem<Module,bool>* ri = cfg->modules(); ri != NULL; ri = ri->next)
			{
				// Grab Module pointer
				Module* module = ri->item;

				result = module->process(*this, cfg->processPool());

				if (!result) break;
			}
			// TEST - Calculate energy of current system, partial data, write, and quit
			//totalEnergyTest(cfg);
			//totalEnergy(worldPool_, cfg);
			//intramolecularEnergy(worldPool_, cfg);
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		/*
		 *  3)	Reassemble all Configuration data on all nodes
		 */
		Messenger::print("\n");
		Messenger::print("===== Reassemble Data\n");
		Messenger::print("\n");
		// TODO


		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		/*
		 *  4)	Run Modules for all Samples (using worldPool_)
		 */
		Messenger::print("\n");
		Messenger::print("===== Sample Processing\n");
		Messenger::print("\n");

		// TODO


		/*
		 *  5)	Perform post-processing tasks (using worldPool_)
		 */
		Messenger::print("\n");
		Messenger::print("===== Post-Processing\n");
		for (RefListItem<Module,bool>* ri = postProcessingTasks_.first(); ri != NULL; ri = ri->next)
		{
			// Grab Module pointer
			Module* module = ri->item;

			Messenger::print("\n");
			Messenger::print("--> Module '%s'\n", module->name());

			// Execute the post-processing stage
			if (!module->postProcess(*this, worldPool_))
			{
				Messenger::error("Module '%s' experienced errors. Exiting now.\n", module->name());
				return false;
			}
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

		Messenger::print("\n");
		Messenger::print("========================================\n");
		Messenger::print("  END OF MAIN LOOP ITERATION %10i\n", iteration);
		Messenger::print("========================================\n");
		Messenger::print("\n");
	}
	while (iteration < maxIterations_);

	return true;
}

