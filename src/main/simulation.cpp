// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/species.h"
#include "kernels/producer.h"
#include "main/dissolve.h"
#include <cstdio>

// Set frequency with which to write various iteration data
void Dissolve::setRestartFileFrequency(int n) { restartFileFrequency_ = n; }

// Return frequency with which to write restart file
int Dissolve::restartFileFrequency() const { return restartFileFrequency_; }

// Prepare for main simulation
bool Dissolve::prepare()
{
    Messenger::banner("Preparing Simulation");

    // Check Species
    for (const auto &sp : coreData_.species())
    {
        if (!sp->checkSetUp())
            return false;
    }

    // Store / update last-used pair potential cutoff
    // If lastPairPotentialCutoff is nullopt, store the current value and move on leaving the cutoff to use as nullopt.
    static std::optional<double> lastPairPotentialRange;
    std::optional<double> newPairPotentialRange;
    if (!lastPairPotentialRange)
        lastPairPotentialRange = PairPotential::range();
    else if (lastPairPotentialRange != PairPotential::range())
    {
        lastPairPotentialRange = PairPotential::range();
        newPairPotentialRange = PairPotential::range();
    }

    // Make sure pair potentials are up-to-date
    if (!updatePairPotentials())
        return false;

    // // Check Configurations
    // std::set<const Species *> globalUsedSpecies;
    // for (auto &cfg : coreData_.configurations())
    // {
    //     // If the configuration is empty, initialise it now
    //     if (cfg->nMolecules() == 0)
    //     {
    //         if (!cfg->initialiseContent({*this}))
    //             return Messenger::error("Failed to initialise content for configuration '{}'.\n", cfg->name());
    //     }
    //     else if (newPairPotentialRange)
    //         cfg->updateCells();
    //
    //     // Check Box extent against pair potential range
    //     auto maxPPRange = cfg->box()->inscribedSphereRadius();
    //     if (PairPotential::range() > maxPPRange)
    //         return Messenger::error("PairPotential range ({}) is longer than the shortest non-minimum image distance
    //         ({}).\n",
    //                                 PairPotential::range(), maxPPRange);
    //
    //     // Update species usage for the next check
    //     for (auto &[sp, pop] : cfg->speciesPopulations())
    //         globalUsedSpecies.emplace(sp);
    // }
    // If we have no configurations, check all species regardless
    // if (coreData_.nConfigurations() == 0)
    // for (const auto &sp : coreData_.species())
    // globalUsedSpecies.emplace(sp.get());
    // TODO DISSOLVE2

    // Set up all modules and return
    return coreData_.setUpProcessingLayerModules(*this);
}

// Iterate main simulation
bool Dissolve::iterate(int nIterations)
{
    iterationTimer_.start();

    for (auto iter = 0; iter < nIterations; ++iter)
    {
        // Increase iteration counters
        ++iteration_;
        ++nIterationsPerformed_;

        Messenger::banner(" START MAIN LOOP ITERATION {:10d}         {}", iteration_, DissolveSys::currentTimeAndDate());

        /*
         *  0)	Print schedule of tasks to run
         */
        auto thisTime = 0.0;
        auto nEnabledModules = 0;

        for (auto &layer : coreData_.processingLayers())
        {
            Messenger::print("Processing layer '{}'  ({}):\n\n", layer->name(), layer->frequencyDetails(iteration_));

            if (!layer->isEnabled())
                continue;

            auto layerExecutionCount = iteration_ / layer->frequency();
            for (auto &module : layer->modules())
            {
                Messenger::print("      --> {:20}  ({})\n", module->name(), module->frequencyDetails(layerExecutionCount));

                if (module->isEnabled())
                    ++nEnabledModules;

                thisTime += module->processTimes().value();
            }
            Messenger::print("\n");
        }

        // If no modules are enabled, complain that we have nothing to do!
        if (nEnabledModules == 0)
            return Messenger::error("No modules or layers enabled - nothing to do!\n");

        /*
         *  1)	Loop over Configurations and perform any upkeep tasks
         */
        Messenger::banner("Configuration Upkeep");

        for (auto &cfg : coreData_.configurations())
        {
            Messenger::heading("'{}'", cfg->name());

            auto kernel = KernelProducer::energyKernel(cfg.get(), potentialMap_);

            // Apply the current size factor
            cfg->applySizeFactor(kernel.get());
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of Configuration upkeep...\n");

        /*
         *  2)	Run processing Modules
         */
        for (auto &layer : coreData_.processingLayers())
        {
            // Check if this layer is due to run this iteration
            if (!layer->runThisIteration(iteration_))
                continue;

            Messenger::banner("Layer '{}'", layer->name());
            auto layerExecutionCount = iteration_ / layer->frequency();

            // Check run-control settings
            if (!layer->canRun(processingModuleData_))
                continue;

            for (auto &module : layer->modules())
            {
                if (!module->runThisIteration(layerExecutionCount))
                    continue;

                Messenger::heading("{} ({})", ModuleTypes::moduleType(module->type()), module->name());

                if (module->executeProcessing(*this) == Module::ExecutionResult::Failed)
                    return Messenger::error("Module '{}' experienced problems. Exiting now.\n", module->name());
            }
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of main processing...\n");

        /*
         *  3)	Write restart file.
         */
        if ((restartFileFrequency_ > 0) && (iteration_ % restartFileFrequency_ == 0))
        {
            Messenger::banner("Write Restart File");

            // Flag other data for inclusion in restart file
            // -- Iteration number
            processingModuleData_.realise<int>("Iteration", "Dissolve", GenericItem::InRestartFileFlag) = iteration_;
            // -- Pair Potentials
            for (auto &&[at1, at2, pot] : pairPotentials_)
            {
                processingModuleData_.realise<Data1D>(std::format("Potential_{}-{}_Additional", at1->name(), at2->name()),
                                                      "Dissolve", GenericItem::InRestartFileFlag) = pot->additionalPotential();
            }

            // Check and remove restart file backup
            std::string restartFileBackup = std::format("{}.prev", restartFilename_);
            if (DissolveSys::fileExists(restartFileBackup) && (std::remove(restartFileBackup.c_str()) != 0))
            {
                Messenger::error("Could not remove old restart file backup.\n");
                return false;
            }

            // Rename current restart file (if it exists)
            if (DissolveSys::fileExists(restartFilename_) &&
                (std::rename(restartFilename_.c_str(), restartFileBackup.c_str()) != 0))
            {
                Messenger::error("Could not rename current restart file.\n");
                return false;
            }

            // Save new restart file
            Timer saveRestartTimer;
            saveRestartTimer.start();

            if (!saveRestart(restartFilename_))
            {
                Messenger::error("Failed to write restart file.\n");
                return false;
            }

            saveRestartTimer.stop();
            saveRestartTimes_ += saveRestartTimer.secondsElapsed();
        }

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

// Estimate time in seconds required to perform next n steps (if possible to determine)
std::optional<double> Dissolve::estimateRequiredTime(int nIterations)
{
    auto seconds = 0.0;
    auto n = 0;

    for (const auto &layer : coreData_.processingLayers())
    {
        if (!layer->isEnabled())
            continue;

        // Determine how many times this layer will run in the provided number of iterations
        auto nLayer = ((iteration_ % layer->frequency()) + nIterations) / layer->frequency();
        if (nLayer == 0)
            continue;

        // Determine the iteration count of the layer
        auto layerIteration = iteration_ / layer->frequency();

        // For each module in the layer, determine how many times it will run in the number of layer iterations
        for (const auto &module : layer->modules())
        {
            auto nModule = ((layerIteration % module->frequency()) + nLayer) / module->frequency();
            if (nModule == 0)
                continue;

            // Do we have valid timing information for the module?
            if (module->processTimes().count() > 0)
            {
                ++n;
                seconds += nModule * module->processTimes();
            }
        }
    }

    return n > 0 ? std::optional<double>(seconds) : std::optional<double>();
}

// Print timing information
void Dissolve::printTiming()
{
    Messenger::banner("Timing Information");

    // Determine format for timing information output, accounting for the longest Module name we have
    auto maxLength = 0;
    for (const auto *module : coreData_.moduleInstances())
    {
        const auto length = module->name().size();
        if (length > maxLength)
            maxLength = length;
    }

    // Add on space for brackets
    maxLength += 2;

    for (auto &layer : coreData_.processingLayers())
    {
        Messenger::print("Accumulated timing for layer '{}':\n\n", layer->name());
        for (auto &module : layer->modules())
        {
            SampledDouble timingInfo = module->processTimes();
            Messenger::print("      --> {:>20}  {:<{}}  {:7.2g} s/iter  ({} iterations)",
                             ModuleTypes::moduleType(module->type()), std::format("({})", module->name()), maxLength,
                             timingInfo.value(), timingInfo.count());
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
