/*
    *** Dissolve - Simulation
    *** src/main/simulation.cpp
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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include <cstdio>
#include <numeric>

// Set random seed
void Dissolve::setSeed(int seed) { seed_ = seed; }

// Return random seed
int Dissolve::seed() const { return seed_; }

// Set frequency with which to write various iteration data
void Dissolve::setRestartFileFrequency(int n) { restartFileFrequency_ = n; }

// Return frequency with which to write restart file
int Dissolve::restartFileFrequency() const { return restartFileFrequency_; }

// Prepare for main simulation
bool Dissolve::prepare()
{
    Messenger::banner("Preparing Simulation");

    // Initialise random seed
    if (seed_ == -1)
        srand((unsigned)time(nullptr));
    else
        srand(seed_);

    // Check Species
    for (auto *sp = species().first(); sp != nullptr; sp = sp->next())
        if (!sp->checkSetUp())
            return false;

    // Reassign AtomType indices (in case one or more have been added / removed)
    auto count = 0;
    for (auto at : atomTypes())
        at->setIndex(count++);

    // Check Configurations
    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        // Check Box extent against pair potential range
        auto maxPPRange = cfg->box()->inscribedSphereRadius();
        if (pairPotentialRange_ > maxPPRange)
        {
            Messenger::error("PairPotential range ({}) is longer than the shortest non-minimum image distance ({}).\n",
                             pairPotentialRange_, maxPPRange);
            return false;
        }

        // Check total charge of Configuration
        auto totalQ = 0.0;
        if (pairPotentialsIncludeCoulomb_)
        {
            auto &types = cfg->usedAtomTypesList();
            totalQ = std::accumulate(types.begin(), types.end(), totalQ, [](auto acc, const auto &atd) {
                return acc + atd.population() * atd.atomType()->parameters().charge();
            });
        }
        else
        {
            ListIterator<SpeciesInfo> spInfoIterator(cfg->usedSpecies());
            while (auto *spInfo = spInfoIterator.iterate())
                totalQ += spInfo->species()->totalChargeOnAtoms() * spInfo->population();
        }
        if (fabs(totalQ) > 1.0e-5)
            return Messenger::error("Total charge for Configuration '{}' is non-zero ({:e}). Refusing to proceed!\n",
                                    cfg->name(), totalQ);
    }

    // Make sure pair potentials are up-to-date
    if (!generatePairPotentials())
        return false;

    // Create PairPotential matrix
    Messenger::print("Creating PairPotential matrix ({}x{})...\n", coreData_.nAtomTypes(), coreData_.nAtomTypes());
    if (!potentialMap_.initialise(coreData_.atomTypes(), pairPotentials_, pairPotentialRange_))
        return false;

    // Check Modules have suitable numbers of Configuration targets
    for (auto *module : moduleInstances_)
    {
        if (module->isDisabled())
            continue;

        if (!module->hasValidNTargetConfigurations(true))
            return false;
    }

    // Set up parallel comms / limits etc.
    if (!setUpMPIPools())
        return Messenger::error("Failed to set up parallel communications.\n");

    return true;
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

    iterationTimer_.zero();
    iterationTimer_.start();

    for (int iter = 0; iter < nIterations; ++iter)
    {
        // Increase iteration counters
        ++iteration_;
        ++nIterationsPerformed_;

        Messenger::banner(" START MAIN LOOP ITERATION {:10d}         {}", iteration_, DissolveSys::currentTimeAndDate());

        /*
         *  0)	Print schedule of tasks to run, and write heartbeat file
         */
        auto thisTime = 0.0;
        auto nEnabledModules = 0;

        for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
        {
            if (cfg->nModules() == 0)
                continue;

            Messenger::print("Configuration layer '{}'  ({}):\n\n", cfg->name(),
                             cfg->moduleLayer().frequencyDetails(iteration_));

            auto layerExecutionCount = iteration_ / cfg->moduleLayer().frequency();
            ListIterator<Module> modIterator(cfg->modules());
            while (Module *module = modIterator.iterate())
            {
                Messenger::print("      --> {:20}  ({})\n", module->type(), module->frequencyDetails(layerExecutionCount));

                if (module->isEnabled())
                    ++nEnabledModules;

                // TODO This will estimate wrongly for anything other than Sequential Processing
                thisTime += module->processTimes().value();
            }
        }
        Messenger::print("\n");

        ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
        while (ModuleLayer *layer = processingLayerIterator.iterate())
        {
            Messenger::print("Processing layer '{}'  ({}):\n\n", layer->name(), layer->frequencyDetails(iteration_));

            if (!layer->enabled())
                continue;

            auto layerExecutionCount = iteration_ / layer->frequency();
            ListIterator<Module> processingIterator(layer->modules());
            while (Module *module = processingIterator.iterate())
            {
                Messenger::print("      --> {:20}  ({})\n", module->type(), module->frequencyDetails(layerExecutionCount));

                if (module->isEnabled())
                    ++nEnabledModules;

                thisTime += module->processTimes().value();
            }
            Messenger::print("\n");
        }

        // If no modules are enabled, complain that we have nothing to do!
        if (nEnabledModules == 0)
            return Messenger::error("No modules or layers enabled - nothing to do!\n");

        // Write heartbeat file or display appropriate message
        if (worldPool().isMaster() && (writeHeartBeat()))
        {
            Messenger::print("Write heartbeat file...");

            saveHeartBeat(fmt::format("{}.beat", inputFilename_), thisTime);
        }
        if (!writeHeartBeat())
        {
            Messenger::print("No Heartbeat file will be written.");
        }

        /*
         *  1)	Loop over Configurations and run their modules in the sequence in which they are defined.
         * 	If a process is not involved in the Configuration's ProcessPool, it can move on.
         */
        Messenger::banner("Configuration Processing");

        auto result = true;
        for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
        {
            // Check for failure of one or more processes / processing tasks
            if (!worldPool().allTrue(result))
            {
                Messenger::error("One or more processes experienced failures. Exiting now.\n");
                return false;
            }

            Messenger::heading("'{}'", cfg->name());

            // Perform any necessary actions before we start processing this Configuration's Modules
            // -- Apply the current size factor
            cfg->applySizeFactor(potentialMap_);

            // Check involvement of this process
            if (!cfg->processPool().involvesMe())
            {
                Messenger::print("Process rank {} not involved with this Configuration, so moving on...\n",
                                 ProcessPool::worldRank());
                continue;
            }

            // Loop over Modules defined in the Configuration
            ListIterator<Module> moduleIterator(cfg->modules());
            while (Module *module = moduleIterator.iterate())
            {
                if (!module->runThisIteration(iteration_))
                    continue;

                Messenger::heading("{} ({})", module->type(), module->uniqueName());

                result = module->executeProcessing(*this, cfg->processPool());

                if (!result)
                    return false;
            }
        }
        if (!result)
            return false;

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of Configuration processing...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  2)	Reassemble data on all nodes.
         */
        Messenger::banner("Reassemble Data");
        // Loop over Configurations
        for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
        {
            Messenger::printVerbose("Broadcasting data for Configuration '{}'...\n", cfg->name());
            if (!cfg->broadcastCoordinates(worldPool(), cfg->processPool().rootWorldRank()))
                return false;

            Messenger::printVerbose("Broadcasting Module data for Configuration '{}'...\n", cfg->name());
            if (!cfg->moduleData().broadcast(worldPool(), cfg->processPool().rootWorldRank(), coreData_))
                return false;
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of data reassembly...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  3)	Run processing Modules (using the world pool).
         */
        processingLayerIterator.restart();
        while (ModuleLayer *layer = processingLayerIterator.iterate())
        {
            // Check if this layer is due to run
            if (!layer->runThisIteration(iteration_))
                continue;

            Messenger::banner("Layer '{}'", layer->name());
            auto layerExecutionCount = iteration_ / layer->frequency();

            ListIterator<Module> processingIterator(layer->modules());
            while (auto *module = processingIterator.iterate())
            {
                if (!module->runThisIteration(layerExecutionCount))
                    continue;

                Messenger::heading("{} ({})", module->type(), module->uniqueName());

                result = module->executeProcessing(*this, worldPool());

                if (!result)
                {
                    Messenger::error("Module '{}' experienced problems. Exiting now.\n", module->type());
                    return false;
                }
            }
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of main processing...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  5)	Write restart file.
         */
        if (worldPool().isMaster() && (restartFileFrequency_ > 0) && (iteration_ % restartFileFrequency_ == 0))
        {
            Messenger::banner("Write Restart File");

            /*
             * Flag other data for inclusion in restart file
             */

            // Iteration number
            GenericListHelper<int>::realise(processingModuleData_, "Iteration", "Dissolve", GenericItem::InRestartFileFlag) =
                iteration_;

            // Pair Potentials
            for (auto *pot = pairPotentials_.first(); pot != nullptr; pot = pot->next())
            {
                GenericListHelper<Data1D>::realise(
                    processingModuleData_,
                    fmt::format("Potential_{}-{}_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ()), "Dissolve",
                    GenericItem::InRestartFileFlag) = pot->uAdditional();
            }

            /*
             * Write Restart File
             */

            // If a restart filename isn't currently set, generate one now.
            if (restartFilename_.empty())
                restartFilename_ = fmt::format("{}.restart", inputFilename_);
            std::string restartFileBackup = fmt::format("{}.prev", restartFilename_);

            // Check and remove restart file backup
            if (DissolveSys::fileExists(restartFileBackup) && (std::remove(restartFileBackup.c_str()) != 0))
            {
                Messenger::error("Could not remove old restart file backup.\n");
                worldPool().decideFalse();
                return false;
            }

            // Rename current restart file (if it exists)
            if (DissolveSys::fileExists(restartFilename_) &&
                (std::rename(restartFilename_.c_str(), restartFileBackup.c_str()) != 0))
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

            // All good. Carry on!
            worldPool().decideTrue();
        }
        else if (worldPool().isSlave() && (restartFileFrequency_ > 0) && (iteration_ % restartFileFrequency_ == 0) &&
                 (!worldPool().decision()))
            return false;

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of data write section...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        iterationTime_ += iterationTimer_.split();

        Messenger::banner("END OF MAIN LOOP ITERATION {:10d}         {}", iteration_, DissolveSys::currentTimeAndDate());
    }

    iterationTimer_.stop();

    return true;
}

// Reset current simulation step
void Dissolve::resetIterationCounter() { iteration_ = 0; }

// Return current simulation step
int Dissolve::iteration() const { return iteration_; }

// Return per-iteration time in seconds
double Dissolve::iterationTime() const { return iterationTime_.value(); }

// Print timing information
void Dissolve::printTiming()
{
    Messenger::banner("Timing Information");

    // Determine format for timing information output, accounting for the longest Module name we have
    auto maxLength = 0;
    for (Module *module : moduleInstances_)
    {
        const auto length = module->uniqueName().size();
        if (length > maxLength)
            maxLength = length;
    }

    // Add on space for brackets
    maxLength += 2;

    for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
    {
        if (cfg->nModules() == 0)
            continue;

        Messenger::print("Accumulated timing for Configuration '{}' processing:\n\n", cfg->name());

        ListIterator<Module> modIterator(cfg->modules().modules());
        while (Module *module = modIterator.iterate())
        {
            SampledDouble timingInfo = module->processTimes();
            Messenger::print("      --> {:>20}  {:<{}}  {:7.2g} s/iter  ({} iterations)", module->type(),
                             fmt::format("({})", module->uniqueName()), maxLength, timingInfo.value(), timingInfo.count());
        }

        Messenger::print("\n");
    }

    ListIterator<ModuleLayer> processingLayerIterator(processingLayers_);
    while (ModuleLayer *layer = processingLayerIterator.iterate())
    {
        Messenger::print("Accumulated timing for layer '{}':\n\n", layer->name());
        ListIterator<Module> processingIterator(layer->modules());
        while (Module *module = processingIterator.iterate())
        {
            SampledDouble timingInfo = module->processTimes();
            Messenger::print("      --> {:>20}  {:<{}}  {:7.2g} s/iter  ({} iterations)", module->type(),
                             fmt::format("({})", module->uniqueName()), maxLength, timingInfo.value(), timingInfo.count());
        }

        Messenger::print("\n");
    }

    Messenger::print("Accumulated timing for general upkeep:\n\n");
    Messenger::print("      --> {:>20}  {:<{}}  {:7.2g} s/write ({} writes)", "Restart File", "", maxLength,
                     saveRestartTimes_.value(), saveRestartTimes_.count());
    Messenger::print("\n");

    if (nIterationsPerformed_ == 0)
        Messenger::print("No iterations performed, so no per-iteration timing available.\n");
    else
        Messenger::print("Total time taken for {} iterations was {} ({:.2g} s/iter).\n", nIterationsPerformed_,
                         iterationTimer_.elapsedTimeString(), iterationTime_.value());

    Messenger::print("\n");
}
