// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/species.h"
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
    for (const auto &sp : species())
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
        auto totalQ =
            std::accumulate(cfg->usedSpecies().begin(), cfg->usedSpecies().end(), 0.0, [&](const auto &acc, auto &spInfo) {
                return acc + spInfo.species()->totalCharge(pairPotentialsIncludeCoulomb_) * spInfo.population();
            });
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
    iterationTimer_.zero();
    iterationTimer_.start();

    for (auto iter = 0; iter < nIterations; ++iter)
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
         *  1)	Loop over Configurations and perform any upkeep tasks
         */
        Messenger::banner("Configuration Upkeep");

        for (auto *cfg = configurations().first(); cfg != nullptr; cfg = cfg->next())
        {
            Messenger::heading("'{}'", cfg->name());

            // Perform any necessary actions before we start processing this Configuration's Modules
            // -- Apply the current size factor
            cfg->applySizeFactor(potentialMap_);
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of Configuration processing...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  2)	Run processing Modules (using the world pool).
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

                auto result = module->executeProcessing(*this, worldPool());

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
         *  4)	Write restart file.
         */
        if (worldPool().isMaster() && (restartFileFrequency_ > 0) && (iteration_ % restartFileFrequency_ == 0))
        {
            Messenger::banner("Write Restart File");

            // Flag other data for inclusion in restart file
            // -- Iteration number
            processingModuleData_.realise<int>("Iteration", "Dissolve", GenericItem::InRestartFileFlag) = iteration_;
            // -- Pair Potentials
            for (auto *pot = pairPotentials_.first(); pot != nullptr; pot = pot->next())
            {
                processingModuleData_.realise<Data1D>(
                    fmt::format("Potential_{}-{}_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ()), "Dissolve",
                    GenericItem::InRestartFileFlag) = pot->uAdditional();
            }

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
