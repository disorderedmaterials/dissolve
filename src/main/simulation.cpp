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
#include "modules/export.h"

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
	 *  6)	Write Configuration coordinates (master process only)
	 */

	/*
	 * Start of Main Loop
	 */
	bool keepGoing = true;
	do
	{
		// Increase iteration counter
		++iteration_;

		Messenger::banner("MAIN LOOP ITERATION %10i / %-10s", iteration_, maxIterations_ == -1 ? "(no limit)" : DUQSys::itoa(maxIterations_));

		/*
		 *  0)	Print schedule of tasks to run
		 */
		Messenger::print("--> Pre-Processing\n");
		if (preProcessingTasks_.nItems() == 0) Messenger::print("  (( No Tasks ))\n");
		RefListIterator<Module,bool> preIterator(preProcessingTasks_);
		while (Module* module = preIterator.iterate()) Messenger::print("      --> %-20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));
		Messenger::print("\n");

		Messenger::print("--> Configuration Processing\n");
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			Messenger::print("   * '%s'\n", cfg->name());
			if (cfg->nModules() == 0) Messenger::print("  (( No Tasks ))\n");
			RefListIterator<Module,bool> modIterator(cfg->modules());
			while (Module* module = modIterator.iterate()) Messenger::print("      --> %20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));
		}
		Messenger::print("\n");

		Messenger::print("--> Post-Processing\n");
		if (postProcessingTasks_.nItems() == 0) Messenger::print("  (( No Tasks ))\n");
		RefListIterator<Module,bool> postIterator(postProcessingTasks_);
		while (Module* module = postIterator.iterate()) Messenger::print("      --> %-20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));


		/*
		 *  1) 	Perform pre-processing tasks (using worldPool_)
		 */
		if (preProcessingTasks_.nItems() > 0) Messenger::banner("Pre-Processing");
		preIterator.restart();
		while (Module* module = preIterator.iterate())
		{
			if (!module->runThisIteration(iteration_)) continue;

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
		Messenger::banner("Configuration Processing");

		bool result = true;
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			// Check for failure of one or more processes / processing tasks
			if (!worldPool_.allTrue(result))
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
			RefListIterator<Module,bool> moduleIterator(cfg->modules());
			while (Module* module = moduleIterator.iterate())
			{
				if (!module->runThisIteration(iteration_)) continue;

				Messenger::print("\n");
				result = module->process(*this, cfg->processPool());

				if (!result) break;
			}
		}

		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);


		/*
		 *  3)	Reassemble data on all nodes
		 */
		Messenger::banner("Reassemble Data");
		// Loop over Configurations
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			if (!cfg->broadcastCoordinates(worldPool_, cfg->processPool().rootWorldRank())) return false;
		}
		// Module-centred data
		RefListIterator<Module,bool> moduleIterator(ModuleList::masterInstances());
		while (Module* module = moduleIterator.iterate()) if (!module->broadcastData(*this, worldPool_)) return false;
		
		// Sync up all processes
		worldPool_.wait(ProcessPool::Pool);

	
		/*
		 *  4)	Run Modules for all Samples (using worldPool_)
		 */
		Messenger::banner("Sample Processing");

		// TODO


		/*
		 *  5)	Perform post-processing tasks (using worldPool_)
		 */
		if (postProcessingTasks_.nItems() > 0) Messenger::print("Post-Processing");
		postIterator.restart();
		while (Module* module = postIterator.iterate())
		{
			if (!module->runThisIteration(iteration_)) continue;

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


		/*
		 *  6)	Write Configuration data
		 */
		if (worldPool_.isMaster())
		{
			Messenger::print("\n");
			Messenger::print("===== Write Configuration Data\n");
			Messenger::print("\n");
			for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
			{
				if (iteration_%cfg->coordinatesOutputFrequency() != 0) continue;

				Messenger::print("Writing Configuration output file '%s'...\n", cfg->outputCoordinatesFile());

				// Open the file and use the Export module to write the Configuration data as xyz
				LineParser parser;
				if (!parser.openOutput(cfg->outputCoordinatesFile(), true))
				{
					parser.closeFiles();
					worldPool_.stop();
					return false;
				}
				else if (!Export::writeConfigurationXYZ(parser, cfg, cfg->name()))
				{
					Messenger::print("Export: Failed to write Configuration output file.\n");
					parser.closeFiles();
					worldPool_.stop();
					return false;
				}

			}
		}
		else if (!worldPool_.decision()) return false;

		Messenger::banner("END OF MAIN LOOP ITERATION %10i", iteration_);
	}
	while ((maxIterations_ < 0) || (iteration_ < maxIterations_));

	return true;
}

// Return current simulation step
int DUQ::iteration()
{
	return iteration_;
}
