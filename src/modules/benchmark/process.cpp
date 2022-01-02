// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/regionaldistributor.h"
#include "io/export/data1d.h"
#include "io/import/data1d.h"
#include "main/dissolve.h"
#include "modules/benchmark/benchmark.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"

// Run main processing
bool BenchmarkModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Get options
    Messenger::print("Benchmark: Test timings will be averaged over {} {}.\n", nRepeats_, nRepeats_ == 1 ? "run" : "runs");
    Messenger::print("Benchmark: Test timings {} be saved to disk.\n", save_ ? "will" : "will not");
    Messenger::print("\n");

    // Loop over target Configurations
    for (auto *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());
        ProcessPool::DivisionStrategy strategy = procPool.bestStrategy();

        /*
         * Configuration Generation
         */
        if (testGenerator_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                srand(dissolve.seed());

                Timer timer;
                Messenger::mute();
                cfg->generate(procPool, dissolve.pairPotentialRange());
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "Generator"),
                              "Configuration generator", timing, save_);
        }

        /*
         * RDF Calculation - Cells method, to maximum range allowed by box
         */
        if (testRDFCells_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                RDFModule rdfModule;
                rdfModule.keywords().set("Configuration", cfg);
                cfg->incrementContentsVersion();
                srand(dissolve.seed());

                // Run the Module calculation
                bool upToDate;
                Timer timer;
                Messenger::mute();
                rdfModule.calculateGR(dissolve.processingModuleData(), procPool, cfg, RDFModule::CellsMethod,
                                      cfg->box()->inscribedSphereRadius(), 0.05, upToDate);
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "RDFCells"),
                              "RDF (Cells) to half-cell limit", timing, save_);
        }

        /*
         * RDF Calculation - Simple method, to maximum range allowed by box
         */
        if (testRDFSimple_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                RDFModule rdfModule;
                rdfModule.keywords().set("Configuration", cfg);
                cfg->incrementContentsVersion();
                srand(dissolve.seed());

                // Run the Module calculation
                bool upToDate;
                Timer timer;
                Messenger::mute();
                rdfModule.calculateGR(dissolve.processingModuleData(), procPool, cfg, RDFModule::SimpleMethod,
                                      cfg->box()->inscribedSphereRadius(), 0.05, upToDate);
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "RDFSimple"),
                              "RDF (Simple) to half-cell limit", timing, save_);
        }

        /*
         * Energy Calculation - Intramolecular Terms
         */
        if (testIntraEnergy_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                Timer timer;
                Messenger::mute();
                EnergyModule::intraMolecularEnergy(procPool, cfg, dissolve.potentialMap());
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "IntraEnergy"),
                              "Intramolecular energy", timing, save_);
        }

        /*
         * Energy Calculation - Intermolecular Terms
         */
        if (testInterEnergy_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                Timer timer;
                Messenger::mute();
                EnergyModule::interAtomicEnergy(procPool, cfg, dissolve.potentialMap());
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "InterEnergy"), "Interatomic energy",
                              timing, save_);
        }

        /*
         * Distributors
         */
        if (testDistributors_)
        {
            SampledDouble timing;
            for (auto n = 0; n < nRepeats_; ++n)
            {
                // Create a Molecule distributor
                RegionalDistributor distributor(cfg->nMolecules(), cfg->cells(), procPool, strategy);

                Timer timer;
                Messenger::mute();
                while (distributor.cycle())
                {
                    // Get next set of Molecule targets from the distributor
                    auto targetMolecules = distributor.assignedMolecules();

                    // Switch parallel strategy if necessary
                    if (distributor.currentStrategy() != strategy)
                    {
                        // Set the new strategy
                        strategy = distributor.currentStrategy();

                        // Re-initialise the random buffer
                        procPool.initialiseRandomBuffer(ProcessPool::subDivisionStrategy(strategy));
                    }

                    // Loop over target Molecules
                    for (auto &molId : targetMolecules)
                        // Get Molecule index and pointer
                        auto mol = cfg->molecule(molId);
                }
                Messenger::unMute();
                timing += timer.split();
            }
            printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), cfg->niceName(), "RegionalDist"),
                              "Distributor (regional)", timing, save_);
        }
    }

    return true;
}

// Print timing information, assessing it against last value in existing timings (if found)
void BenchmarkModule::printTimingResult(std::string_view testFile, std::string_view testDescription,
                                        const SampledDouble &timing, bool storeNewTiming)
{
    auto existingDataAvailable = false;

    // Open the existing timings file if it exists
    Data1D existingTimings;
    existingTimings.addErrors();
    Data1DImportFileFormat importer(testFile, Data1DImportFileFormat::Data1DImportFormat::XY);
    if (importer.fileExists())
    {
        importer.keywords().set("Error", 3);
        existingDataAvailable = importer.importData(existingTimings);
        if (existingDataAvailable)
            existingDataAvailable = existingTimings.nValues() > 0;
    }

    // Print timing, comparing to last known value if available
    if (existingDataAvailable)
    {
        SampledDouble lastTiming = existingTimings.values().back();
        double deltaT = lastTiming.value() - timing.value();
        Messenger::print("  {:50}  {:8.4e} s (+/- {:8.4e} s) => {}{:0.3e} s ({}{:0.2f}%)\n", testDescription, timing.value(),
                         timing.stDev(), deltaT < timing.value() ? '+' : '-', fabs(deltaT), deltaT < timing.value() ? '+' : '-',
                         fabs((deltaT / timing.value()) * 100.0));
    }
    else
        Messenger::print("  {:50}  {:8.4e} s (+/- {:8.4e} s)\n", testDescription, timing.value(), timing.stDev());

    // Store new timing?
    if (storeNewTiming)
    {
        existingTimings.addPoint(existingTimings.nValues() + 1, timing.value(), timing.stDev());

        Data1DExportFileFormat exporter(testFile, Data1DExportFileFormat::Data1DExportFormat::XY);
        exporter.exportData(existingTimings);
    }
}
