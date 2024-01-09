// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/intraShake/intraShake.h"
#include <cstdio>
#include <numeric>

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

        sp->setUpScaledInteractions();
    }

    // Remove unused atom types
    coreData_.removeUnusedAtomTypes();

    // Store / update last-used pair potential cutoff
    // If lastPairPotentialCutoff is nullopt, store the current value and move on leaving the cutoff to use as nullopt.
    static std::optional<double> lastPairPotentialRange;
    std::optional<double> newPairPotentialRange;
    if (!lastPairPotentialRange)
        lastPairPotentialRange = pairPotentialRange_;
    else if (lastPairPotentialRange != pairPotentialRange_)
    {
        lastPairPotentialRange = pairPotentialRange_;
        newPairPotentialRange = pairPotentialRange_;
    }

    // Make sure pair potentials are up-to-date
    if (!regeneratePairPotentials())
        return false;

    // Check Configurations
    std::set<const Species *> globalUsedSpecies;
    for (auto &cfg : coreData_.configurations())
    {
        // If the configuration is empty, initialise it now
        if (cfg->nMolecules() == 0)
        {
            if (!cfg->initialiseContent({worldPool_, *this}))
                return Messenger::error("Failed to initialise content for configuration '{}'.\n", cfg->name());
        }
        else if (newPairPotentialRange)
            cfg->updateCells(*newPairPotentialRange);

        // Check Box extent against pair potential range
        auto maxPPRange = cfg->box()->inscribedSphereRadius();
        if (pairPotentialRange_ > maxPPRange)
            return Messenger::error("PairPotential range ({}) is longer than the shortest non-minimum image distance ({}).\n",
                                    pairPotentialRange_, maxPPRange);

        // Update species usage for the next check
        for (auto &[sp, pop] : cfg->speciesPopulations())
            globalUsedSpecies.emplace(sp);
    }

    // If we have no configurations, check all species regardless
    if (coreData_.nConfigurations() == 0)
        for (const auto &sp : coreData_.species())
            globalUsedSpecies.emplace(sp.get());

    // Check pair potential style - first, determine which styles might be valid for use
    // -- Configuration charges must always be zero
    auto neutralConfigsWithPPCharges = std::all_of(coreData_.configurations().begin(), coreData_.configurations().end(),
                                                   [](const auto &cfg) { return fabs(cfg->totalCharge(true)) < 1.0e-5; });
    Messenger::printVerbose("Configuration neutrality if using charges on atom types    : {}\n",
                            DissolveSys::btoa(neutralConfigsWithPPCharges));
    auto neutralConfigsWithSpeciesCharges = std::all_of(coreData_.configurations().begin(), coreData_.configurations().end(),
                                                        [](const auto &cfg) { return fabs(cfg->totalCharge(false)) < 1.0e-5; });
    Messenger::printVerbose("Configuration neutrality if using charges on species atoms : {}\n",
                            DissolveSys::btoa(neutralConfigsWithSpeciesCharges));

    // -- Do all used Species have 95% non-zero atomic charges?
    auto speciesHaveValidAtomicCharges =
        std::all_of(globalUsedSpecies.begin(), globalUsedSpecies.end(),
                    [](const auto &sp)
                    {
                        return (std::count_if(sp->atoms().begin(), sp->atoms().end(),
                                              [](const auto &i) { return fabs(i.charge()) > 1.0e-5; }) /
                                double(sp->nAtoms())) > 0.95;
                    });
    Messenger::printVerbose("Species atomic charge validity  : {}\n", DissolveSys::btoa(speciesHaveValidAtomicCharges));
    // -- Do all atom types have 95% non-zero charges
    auto atomTypesHaveValidAtomicCharges = (std::count_if(coreData_.atomTypes().begin(), coreData_.atomTypes().end(),
                                                          [](const auto &at) { return fabs(at->charge()) > 1.0e-5; }) /
                                            double(coreData_.nAtomTypes())) > 0.95;
    Messenger::printVerbose("AtomType atomic charge validity : {}\n", DissolveSys::btoa(atomTypesHaveValidAtomicCharges));

    if (automaticChargeSource_)
    {
        // Prefer charges on atom types as this is more efficient
        if (neutralConfigsWithPPCharges && atomTypesHaveValidAtomicCharges)
        {
            atomTypeChargeSource_ = true;
            Messenger::print("[AUTO] Pair potentials will include Coulomb terms - charges will be taken from atom types.\n");
        }
        else if (neutralConfigsWithSpeciesCharges && speciesHaveValidAtomicCharges)
        {
            atomTypeChargeSource_ = false;
            Messenger::print(
                "[AUTO] Pair potentials will not include Coulomb terms - charges will be taken from species atoms.\n");
        }
        else
            return Messenger::error("Current charges (or lack thereof) assigned to atom types and species prevent "
                                    "automatic\ndetermination of a pair potential scheme. Please check your setup!\n");
    }
    else if (atomTypeChargeSource_)
    {
        // User-selected choice is to embed charges from atom types into the pair potential
        if (neutralConfigsWithPPCharges && atomTypesHaveValidAtomicCharges)
            Messenger::print("[MANUAL] Pair potentials will include Coulomb terms - charges will be taken from atom types.\n");
        else if (!neutralConfigsWithPPCharges)
        {
            Messenger::error("Atom type charges in pair potentials requested, but at least one configuration is not "
                             "neutral with this approach.\n");
            for (const auto &cfg : coreData_.configurations())
                Messenger::print("Total charge in configuration '{}' is {}.\n", cfg->name(), cfg->totalCharge(true));
            return false;
        }
        else if (forceChargeSource_)
            Messenger::warn(
                "Atom type charges in pair potentials requested, but some atom types have zero charge which is suspicious.\n");
        else
            return Messenger::error(
                "Atom type charges in pair potentials requested, but some atom types have zero charge which is suspicious.\n"
                "If this is correct, select the 'Force' option in the Forcefield tab or add 'ForceChargeSource True' to the\n"
                "PairPotentials block in your input file to proceed with this choice.\n");
    }
    else
    {
        // User-selected choice is to use charges from species atoms (i.e. no charge contributions to pair potentials)
        if (neutralConfigsWithSpeciesCharges && speciesHaveValidAtomicCharges)
            Messenger::print(
                "[MANUAL] Pair potentials will not include Coulomb terms - charges will be taken from species atoms.\n");
        else if (!neutralConfigsWithSpeciesCharges)
        {
            Messenger::error("Ths use of species atom charges was requested, but at least one configuration is not "
                             "neutral with this approach.\n");
            for (const auto &cfg : coreData_.configurations())
                Messenger::print("Total charge in configuration '{}' is {}.\n", cfg->name(), cfg->totalCharge(false));
            return false;
        }
        else if (forceChargeSource_)
            Messenger::warn("The use of species atom charges was requested, but some have zero charge which is suspicious.\n");
        else
            return Messenger::error(
                "The use of species atom charges was requested, but some have zero charge which is suspicious.\n"
                "If this is correct, select the 'Force' option in the Forcefield tab or add 'ForceChargeSource True' to the\n"
                "PairPotentials block in your input file to proceed with this choice.\n");
    }

    // Make sure pair potentials are up-to-date
    if (!regeneratePairPotentials())
        return false;

    // Generate attached atom lists if IntraShake modules are present and enabled
    auto intraShakeModules = Module::allOfType(ModuleTypes::IntraShake);
    if (!intraShakeModules.empty())
    {
        Messenger::print("Generating attached atom lists for required species...");
        for (auto *module : intraShakeModules)
        {
            auto *cfg = dynamic_cast<IntraShakeModule *>(module)->keywords().getConfiguration("Configuration");
            for (auto &sp : coreData_.species())
                if (cfg->containsSpecies(sp.get()) && !sp->attachedAtomListsGenerated())
                {
                    Messenger::print("Performing one-time generation of attached atom lists for intramolecular "
                                     "terms in Species '{}'...\n",
                                     sp->name());
                    if (sp->nAtoms() > 500)
                        Messenger::warn("'{}' is a large molecule - this might take a while! Consider using a "
                                        "different evolution module.\n",
                                        sp->name());
                    sp->generateAttachedAtomLists();
                }
        }
    }

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

            // Apply the current size factor
            cfg->applySizeFactor(worldPool(), potentialMap_);
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of Configuration upkeep...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  2)	Run processing Modules (using the world pool).
         */
        ModuleContext context(worldPool(), *this);
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

                if (module->executeProcessing(context) == Module::ExecutionResult::Failed)
                    return Messenger::error("Module '{}' experienced problems. Exiting now.\n", module->name());
            }
        }

        // Sync up all processes
        Messenger::printVerbose("Waiting for other processes at end of main processing...\n");
        worldPool().wait(ProcessPool::PoolProcessesCommunicator);

        /*
         *  3)	Write restart file.
         */
        if (worldPool().isMaster() && (restartFileFrequency_ > 0) && (iteration_ % restartFileFrequency_ == 0))
        {
            Messenger::banner("Write Restart File");

            // Flag other data for inclusion in restart file
            // -- Iteration number
            processingModuleData_.realise<int>("Iteration", "Dissolve", GenericItem::InRestartFileFlag) = iteration_;
            // -- Pair Potentials
            for (auto &pot : pairPotentials_)
            {
                processingModuleData_.realise<Data1D>(
                    fmt::format("Potential_{}-{}_Additional", pot->atomTypeNameI(), pot->atomTypeNameJ()), "Dissolve",
                    GenericItem::InRestartFileFlag) = pot->uAdditional();
            }

            // Check and remove restart file backup
            std::string restartFileBackup = fmt::format("{}.prev", restartFilename_);
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
    for (const auto *module : Module::instances())
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
                             ModuleTypes::moduleType(module->type()), fmt::format("({})", module->name()), maxLength,
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
