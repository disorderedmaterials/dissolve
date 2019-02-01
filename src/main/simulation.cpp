/*
	*** Dissolve - Simulation
	*** src/main/simulation.cpp
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

#include "main/dissolve.h"
#include "modules/export/export.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"
#include <cstdio>

/*
 * Public Functions
 */

// Set number of test points to use when calculating Box normalisation arrays
void Dissolve::setNBoxNormalisationPoints(int nPoints)
{
	nBoxNormalisationPoints_ = nPoints;
}

// Return number of test points to use when calculating Box normalisation arrays
int Dissolve::nBoxNormalisationPoints() const
{
	return nBoxNormalisationPoints_;
}

// Set random seed
void Dissolve::setSeed(int seed)
{
	seed_ = seed;
}

// Return random seed
int Dissolve::seed() const
{
	return seed_;
}

// Set frequency with which to write various iteration dat
void Dissolve::setRestartFileFrequency(int n)
{
	restartFileFrequency_ = n;
}

// Return frequency with which to write restart file
int Dissolve::restartFileFrequency() const
{
	return restartFileFrequency_;
}

// Add new processing layer
ModuleLayer* Dissolve::addProcessingLayer()
{
	return processingLayers_.add();
}

// Find named processing layer
ModuleLayer* Dissolve::findProcessingLayer(const char* name) const
{
	ListIterator<ModuleLayer> layerIterator(processingLayers_);
	while (ModuleLayer* layer = layerIterator.iterate()) if (DissolveSys::sameString(layer->name(), name)) return layer;

	return NULL;
}

// Own the specified processing layer
bool Dissolve::ownProcessingLayer(ModuleLayer* layer)
{
	// Sanity check - do we already own this Configuration?
	if (processingLayers_.contains(layer)) return Messenger::error("Already own ModuleLayer '%s', so nothing to do.\n", layer->name());

	processingLayers_.own(layer);

	setUp_ = false;

	return true;
}

// Return number of defined processing layers
int Dissolve::nProcessingLayers() const
{
	return processingLayers_.nItems();
}

// Generate unique processing layer name with base name provided
const char* Dissolve::uniqueProcessingLayerName(const char* base) const
{
	static CharString uniqueName;
	CharString baseName = base;
	uniqueName = baseName;
	int suffix = 0;

	// Must always have a baseName
	if (baseName.isEmpty()) baseName = "Unnamed";

	// Find an unused name starting with the baseName provided
	while (findProcessingLayer(uniqueName))
	{
		// Increase suffix value and regenerate uniqueName from baseName
		++suffix;
		uniqueName.sprintf("%s%i", baseName.get(), suffix);
	}

	return uniqueName;
}

// Return list of processing layers
List<ModuleLayer>& Dissolve::processingLayers()
{
	return processingLayers_;
}

// Return data associated with processing Modules
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
	 *  1)	Run all Modules assigned to Configurations using individual parallel strategies for Configurations
	 *  2)	Reassemble Configuration data on all processes
	 *  3)	Run all processing Modules using all processes available (worldPool_)
	 *  4)	Run analysis processing Modules
	 *  5)	Write restart file (master process only)
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

		Messenger::print("Configuration Processing\n");
		for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
		{
			Messenger::print("   * '%s'\n", cfg->name());
			if (cfg->nModules() == 0) Messenger::print("  (( No Tasks ))\n");
			ListIterator<Module> modIterator(cfg->modules());
			while (Module* module = modIterator.iterate())
			{
				Messenger::print("      --> %20s  (%s)\n", module->type(), module->enabled() ? module->frequencyDetails(iteration_) : "Disabled");

				// TODO This will estimate wrongly for anything other than Sequential Processing
				thisTime += module->processTimes().value();
			}
		}
		Messenger::print("\n");

		ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
		while (ModuleLayer* layer = processingLayerIterator.iterate())
		{
			Messenger::print("Processing layer '%s'  (%s):\n\n", layer->name(), layer->frequencyDetails(iteration_));
			int layerExecutionCount = iteration_ / layer->frequency();

			ListIterator<Module> processingIterator(layer->modules());
			while (Module* module= processingIterator.iterate())
			{
				Messenger::print("      --> %20s  (%s)\n", module->type(), module->frequencyDetails(layerExecutionCount));

				thisTime += module->processTimes().value();
			}
			Messenger::print("\n");
		}

		// Write heartbeat file
		if (worldPool().isMaster())
		{
			Messenger::print("Write heartbeat file...");

			CharString heartBeatFile("%s.beat", filename_.get());

			saveHeartBeat(heartBeatFile, thisTime);
		}


		/*
		 *  1)	Loop over Configurations and run their modules in the sequence in which they are defined.
		 * 	If a process is not involved in the Configuration's ProcessPool, it can move on.
		 */
		Messenger::banner("Configuration Processing");

		bool result = true;
		for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
		{
			// Check for failure of one or more processes / processing tasks
			if (!worldPool().allTrue(result))
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
			ListIterator<Module> moduleIterator(cfg->modules());
			while (Module* module = moduleIterator.iterate())
			{
				if (!module->runThisIteration(iteration_)) continue;

				Messenger::heading("%s (%s)", module->type(), module->uniqueName());

				result = module->executeProcessing(*this, cfg->processPool());

				if (!result) return false;
			}
		}
		if (!result) return false;

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of Configuration processing...\n");
		worldPool().wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  2)	Reassemble data on all nodes.
		 */
		Messenger::banner("Reassemble Data");
		// Loop over Configurations
		for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
		{
			Messenger::printVerbose("Broadcasting data for Configuration '%s'...\n", cfg->name());
			if (!cfg->broadcastCoordinates(worldPool(), cfg->processPool().rootWorldRank())) return false;

			Messenger::printVerbose("Broadcasting Module data for Configuration '%s'...\n", cfg->name());
			if (!cfg->moduleData().broadcast(worldPool(), cfg->processPool().rootWorldRank())) return false;
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of data reassembly...\n");
		worldPool().wait(ProcessPool::PoolProcessesCommunicator);

	
		/*
		 *  3)	Run processing Modules (using the world pool).
		 */
		processingLayerIterator.restart();
		while (ModuleLayer* layer = processingLayerIterator.iterate())
		{
			// Check if this layer is due to run
			if (!layer->runThisIteration(iteration_)) continue;

			Messenger::banner("Layer '%s'", layer->name());
			int layerExecutionCount = iteration_ / layer->frequency();

			ListIterator<Module> processingIterator(layer->modules());
			while (Module* module = processingIterator.iterate())
			{
				if (!module->runThisIteration(layerExecutionCount)) continue;

				Messenger::heading("%s (%s)", module->type(), module->uniqueName());

				result = module->executeProcessing(*this, worldPool());

				if (!result)
				{
					Messenger::error("Module '%s' experienced problems. Exiting now.\n", module->type());
					return false;
				}
			}
		}

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of main processing...\n");
		worldPool().wait(ProcessPool::PoolProcessesCommunicator);


		/*
		 *  5)	Write restart / ensemble data.
		 */
		if (worldPool().isMaster() && (restartFileFrequency_ > 0) && (iteration_%restartFileFrequency_ == 0))
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
				GenericListHelper<Data1D>::realise(processingModuleData_, CharString("Potential_%s-%s_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ()), "Dissolve", GenericItem::InRestartFileFlag) = pot->uAdditional();
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
				worldPool().decideFalse();
				return false;
			}

			// Rename current restart file (if it exists)
			if (DissolveSys::fileExists(restartFilename_) && (rename(restartFilename_, restartFileBackup) != 0))
			{
				Messenger::error("Could not rename current restart file.\n");
				worldPool().decideFalse();
				return false;
			}

			// Save new restart file
			Timer saveRestartTimer;
			saveRestartTimer.start();

			if (!saveRestart(restartFilename_))
			{
				Messenger::error("Failed to write restart file.\n");
				worldPool().decideFalse();
				return false;
			}

			saveRestartTimer.stop();
			saveRestartTimes_ += saveRestartTimer.secondsElapsed();

			/*
			 * Configuration Data
			 */

			// Keep track of number of Configurations saved / to save
			for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
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
						worldPool().decideFalse();
						return false;
					}
					else if (!ExportModule::writeXYZCoordinates(ensembleParser, cfg))
					{
						Messenger::print("Export: Failed to append Configuration ensemble output file.\n");
						ensembleParser.closeFiles();
						worldPool().decideFalse();
						return false;
					}
				}
			}

			// All good. Carry on!
			worldPool().decideTrue();
		}
		else if (worldPool().isSlave() && (restartFileFrequency_ > 0) && (iteration_%restartFileFrequency_ == 0) && (!worldPool().decision())) return false;

		// Sync up all processes
		Messenger::printVerbose("Waiting for other processes at end of data write section...\n");
		worldPool().wait(ProcessPool::PoolProcessesCommunicator);


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
	for (Configuration* cfg = configurations().first(); cfg != NULL; cfg = cfg->next)
	{
		if (cfg->nModules() == 0) continue;

		Messenger::print("   * '%s'\n", cfg->name());
		ListIterator<Module> modIterator(cfg->modules().modules());
		while (Module* module = modIterator.iterate())
		{
			SampledDouble timingInfo = module->processTimes();
			Messenger::print("      --> %30s  %7.2f s/iteration (%i iterations)\n", CharString("%s (%s)", module->type(), module->uniqueName()).get(), timingInfo.value(), timingInfo.count());
		}
	}
	Messenger::print("\n");

	ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
	while (ModuleLayer* layer = processingLayerIterator.iterate())
	{
		Messenger::print("Processing Layer '%s':\n", layer->name());
		ListIterator<Module> processingIterator(layer->modules());
		while (Module* module = processingIterator.iterate())
		{
			SampledDouble timingInfo = module->processTimes();
			Messenger::print("      --> %30s  %7.2f s/iteration (%i iterations)\n", CharString("%s (%s)", module->type(), module->uniqueName()).get(), timingInfo.value(), timingInfo.count());
		}
	}

	Messenger::print("      --> %30s  %7.2f s/write     (%i writes)\n", "Restart File", saveRestartTimes_.value(), saveRestartTimes_.count());
	Messenger::print("\n");


	Messenger::print("Total time taken for %i iterations was %s (%s / iteration).\n", nIterationsPerformed_, mainLoopTimer_.elapsedTimeString(), Timer::timeString(mainLoopTimer_.secondsElapsed() / nIterationsPerformed_));

	Messenger::print("\n");
}
