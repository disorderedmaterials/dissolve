/*
	*** Dissolve - Simulation
	*** src/main/simulation.cpp
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
#include "modules/export/export.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"
#include <cstdio>

/*
 * Private Functions
 */

// Find first occurrence of named Module in pre-processing tasks
Module* Dissolve::findPreProcessingTask(const char* name)
{
	return preProcessingTasks_.find(name);
}

// Find first occurrence of named Module in post-processing tasks
Module* Dissolve::findPostProcessingTask(const char* name)
{
	return postProcessingTasks_.find(name);
}

/*
 * Public Functions
 */

// Return list of main processing Modules to run
ModuleList& Dissolve::processingModules()
{
	return processingModules_;
}

// Return data associated with main processing Modules
GenericList& Dissolve::processingModuleData()
{
	return processingModuleData_;
}

// Iterate main simulation
bool Dissolve::iterate(int nIterations)
{
	/*
	 * This is the main program loop. We perform the following steps:
	 *
	 *  1)	Run all Module pre-processing tasks using all processes available (worldPool_)
	 *  2)	Run all Modules assigned to Configurations using individual parallel strategies for Configurations
	 *  3)	Reassemble Configuration data on all processes
	 *  4)	Run all processing Modules using all processes available (worldPool_)
	 *  5)	Run any Module post-processing tasks using all processes available (worldPool_)
	 *  6)	Write restart file (master process only)
	 */

	if (!setUp_) return Messenger::error("Simulation has not been set up.\n");

	mainLoopTimer_.zero();
	mainLoopTimer_.start();

	for (int iter = 0; iter < nIterations; ++iter)
	{
		// Increase iteration counters
		++iteration_;
		++nIterationsPerformed_;

		Messenger::banner(" START MAIN LOOP ITERATION %10i         %s", iteration_, DissolveSys::currentTimeAndDate());

		/*
		 *  0)	Print schedule of tasks to run, and write heartbeat file
		 */
		double thisTime = 0.0;
		Messenger::print("Pre-Processing\n");
		if (preProcessingTasks_.nModules() == 0) Messenger::print("  (( No Tasks ))\n");
		ListIterator<ModuleReference> preIterator(preProcessingTasks_.modules());
		while (ModuleReference* modRef = preIterator.iterate())
		{
			Module* module = modRef->module();

			Messenger::print("      --> %-20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));

			thisTime += module->processTimes().mean();
		}
		Messenger::print("\n");

		Messenger::print("Configuration Processing\n");
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			Messenger::print("   * '%s'\n", cfg->name());
			if (cfg->nModules() == 0) Messenger::print("  (( No Tasks ))\n");
			ListIterator<ModuleReference> modIterator(cfg->modules().modules());
			while (ModuleReference* modRef = modIterator.iterate())
			{
				Module* module = modRef->module();

				Messenger::print("      --> %20s  (%s)\n", module->name(), module->enabled() ? module->frequencyDetails(iteration_) : "Disabled");

				// TODO This will estimate wrongly for anything other than Sequential Processing
				thisTime += module->processTimes().mean();
			}
		}
		Messenger::print("\n");

		Messenger::print("Main Processing\n");
		ListIterator<ModuleReference> processingIterator(processingModules_.modules());
		while (ModuleReference* modRef = processingIterator.iterate())
		{
			Module* module = modRef->module();
			
			Messenger::print("      --> %20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));

			thisTime += module->processTimes().mean();
		}
		Messenger::print("\n");

		Messenger::print("Post-Processing\n");
		if (postProcessingTasks_.nModules() == 0) Messenger::print("  (( No Tasks ))\n");
		ListIterator<ModuleReference> postIterator(postProcessingTasks_.modules());
		while (ModuleReference* modRef = postIterator.iterate())
		{
			Module* module = modRef->module();
			
			Messenger::print("      --> %-20s  (%s)\n", module->name(), module->frequencyDetails(iteration_));

			thisTime += module->processTimes().mean();
		}

		// Write heartbeat file
		if (worldPool_.isMaster())
		{
			Messenger::print("Write heartbeat file...");

			CharString heartBeatFile("%s.beat", filename_.get());

			saveHeartBeat(heartBeatFile, thisTime);
		}


		/*
		 *  1) 	Perform pre-processing tasks (using worldPool_).
		 */
		if (preProcessingTasks_.nModules() > 0) Messenger::banner("Pre-Processing");
		preIterator.restart();
		while (ModuleReference* modRef = preIterator.iterate())
		{
			Module* module = modRef->module();

			if (!module->runThisIteration(iteration_)) continue;

			Messenger::heading("'%s'", module->name());

			// Execute the pre-processing stage
			if (!module->executePreProcessing(*this, worldPool_))
			{
				Messenger::error("Module '%s' experienced problems. Exiting now.\n", module->name());
				return false;
			}
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of pre-processing...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  2)	Loop over Configurations and run their modules in the sequence in which they are defined.
		 * 	If a process is not involved in the Configuration's ProcessPool, it can move on.
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

			Messenger::heading("'%s'", cfg->name());

			// Check involvement of this process
			if (!cfg->processPool().involvesMe())
			{
				Messenger::print("Process rank %i not involved with this Configuration, so moving on...\n", ProcessPool::worldRank());
				continue;
			}

			// Perform any necessary actions before we start processing this Configuration's Modules
			if (!cfg->prepare(potentialMap_)) return false;

			// Loop over Modules defined in the Configuration
			ListIterator<ModuleReference> moduleIterator(cfg->modules().modules());
			while (ModuleReference* modRef = moduleIterator.iterate())
			{
				Module* module = modRef->module();

				if (!module->runThisIteration(iteration_)) continue;

				Messenger::heading("%s (%s)", module->name(), module->uniqueName());

				result = module->executeMainProcessing(*this, cfg->processPool());

				if (!result) return false;
			}
		}
		if (!result) return false;

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of Configuration processing...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  3)	Reassemble data on all nodes.
		 */
		Messenger::banner("Reassemble Data");
		// Loop over Configurations
		for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
		{
			Messenger::printVerbose("Broadcasting data for Configuration '%s'...\n", cfg->name());
			if (!cfg->broadcastCoordinates(worldPool_, cfg->processPool().rootWorldRank())) return false;

			Messenger::printVerbose("Broadcasting Module data for Configuration '%s'...\n", cfg->name());
			if (!cfg->moduleData().broadcast(worldPool_, cfg->processPool().rootWorldRank())) return false;
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of data reassembly...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);

	
		/*
		 *  4)	Run processing Modules (using worldPool_).
		 */
		if (processingModules_.nModules() > 0) Messenger::banner("Main Processing");
		processingIterator.restart();
		while (ModuleReference* modRef = processingIterator.iterate())
		{
			Module* module = modRef->module();

			if (!module->runThisIteration(iteration_)) continue;

			Messenger::heading("%s (%s)", module->name(), module->uniqueName());

			result = module->executeMainProcessing(*this, worldPool_);

			if (!result)
			{
				Messenger::error("Module '%s' experienced problems. Exiting now.\n", module->name());
				return false;
			}
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of main processing...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  5)	Perform post-processing tasks (using worldPool_).
		 */
		if (postProcessingTasks_.nModules() > 0) Messenger::banner("Post-Processing");
		postIterator.restart();
		while (ModuleReference* modRef = postIterator.iterate())
		{
			Module* module = modRef->module();

			if (!module->runThisIteration(iteration_)) continue;

			Messenger::print("\n");
			Messenger::print("Module '%s'\n", module->name());

			// Execute the post-processing stage
			if (!module->executePostProcessing(*this, worldPool_))
			{
				Messenger::error("Module '%s' experienced problems. Exiting now.\n", module->name());
				return false;
			}
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of post-processing...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  6)	Write restart / ensemble data.
		 */
		if (worldPool_.isMaster() && (restartFileFrequency_ > 0) && (iteration_%restartFileFrequency_ == 0))
		{
			Messenger::banner("Write Restart File");

			/*
			 * Flag other data for inclusion in restart file
			 */
			
			// Iteration number
			GenericListHelper<int>::realise(processingModuleData_, "Iteration", "Dissolve", GenericItem::InRestartFileFlag) = iteration_;

			// Pair Potentials
			for (PairPotential* pot = pairPotentials_.first(); pot != NULL; pot = pot->next)
			{
				GenericListHelper<XYData>::realise(processingModuleData_, CharString("Potential_%s-%s_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ()), "Dissolve", GenericItem::InRestartFileFlag) = pot->uAdditional();
			}

			/*
			 * Restart File
			 */

			// If a restart filename isn't currently set, generate one now.
			if (restartFilename_.isEmpty()) restartFilename_ = CharString("%s.restart", filename_.get());
			CharString restartFileBackup("%s.prev", restartFilename_.get());

			// Check and remove restart file backup
			if (DissolveSys::fileExists(restartFileBackup) && (remove(restartFileBackup) != 0))
			{
				Messenger::error("Could not remove old restart file backup.\n");
				worldPool_.decideFalse();
				return false;
			}

			// Rename current restart file (if it exists)
			if (DissolveSys::fileExists(restartFilename_) && (rename(restartFilename_, restartFileBackup) != 0))
			{
				Messenger::error("Could not rename current restart file.\n");
				worldPool_.decideFalse();
				return false;
			}

			// Save new restart file
			Timer saveRestartTimer;
			saveRestartTimer.start();

			if (!saveRestart(restartFilename_))
			{
				Messenger::error("Failed to write restart file.\n");
				worldPool_.decideFalse();
				return false;
			}

			saveRestartTimer.stop();
			saveRestartTimes_ << saveRestartTimer.secondsElapsed();

			/*
			 * Configuration Data
			 */

			// Keep track of number of Configurations saved / to save
			for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
			{
				// Append ensemble file
				if (cfg->appendEnsemble() && (iteration_%cfg->ensembleFrequency() != 0))
				{
					CharString ensembleFile("%s.xyz.ensemble", cfg->name());
					Messenger::print("Appending Configuration ensemble file '%s'...\n", ensembleFile.get());

					LineParser ensembleParser;
					if (!ensembleParser.appendOutput(ensembleFile.get()))
					{
						ensembleParser.closeFiles();
						worldPool_.decideFalse();
						return false;
					}
					else if (!ExportModule::writeConfigurationXYZ(ensembleParser, cfg, cfg->name()))
					{
						Messenger::print("Export: Failed to append Configuration ensemble output file.\n");
						ensembleParser.closeFiles();
						worldPool_.decideFalse();
						return false;
					}
				}
			}

			// All good. Carry on!
			worldPool_.decideTrue();
		}
		else if (worldPool_.isSlave() && (restartFileFrequency_ > 0) && (iteration_%restartFileFrequency_ == 0) && (!worldPool_.decision())) return false;

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of data write section...\n");
		worldPool_.wait(ProcessPool::PoolProcessesCommunicator);


		Messenger::banner("END OF MAIN LOOP ITERATION %10i         %s", iteration_, DissolveSys::currentTimeAndDate());
	}

	mainLoopTimer_.stop();

	return true;
}

// Return current simulation step
int Dissolve::iteration() const
{
	return iteration_;
}

// Print timing information
void Dissolve::printTiming()
{
	Messenger::banner("Timing Information");

	Messenger::print("Configuration Processing:\n");
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		if (cfg->nModules() == 0) continue;

		Messenger::print("   * '%s'\n", cfg->name());
		ListIterator<ModuleReference> modIterator(cfg->modules().modules());
		while (ModuleReference* modRef = modIterator.iterate())
		{
			Module* module = modRef->module();

			SampledDouble timingInfo = module->processTimes();
			Messenger::print("      --> %30s  %6.1f s/iteration (%i iterations)\n", CharString("%s (%s)", module->name(), module->uniqueName()).get(), timingInfo.mean(), timingInfo.count());
		}
	}
	Messenger::print("\n");

	Messenger::print("Main Processing:\n");
	ListIterator<ModuleReference> processingIterator(processingModules_.modules());
	while (ModuleReference* modRef = processingIterator.iterate())
	{
		Module* module = modRef->module();

		SampledDouble timingInfo = module->processTimes();
		Messenger::print("      --> %30s  %6.1f s/iteration (%i iterations)\n", CharString("%s (%s)", module->name(), module->uniqueName()).get(), timingInfo.mean(), timingInfo.count());
	}
	Messenger::print("      --> %30s  %6.1f s/write     (%i writes)\n", "Restart File", saveRestartTimes_.mean(), saveRestartTimes_.count());
	Messenger::print("\n");


	Messenger::print("Total time taken for %i iterations was %s (%s / iteration).\n", nIterationsPerformed_, mainLoopTimer_.elapsedTimeString(), Timer::timeString(mainLoopTimer_.secondsElapsed() / nIterationsPerformed_));

	Messenger::print("\n");
}
