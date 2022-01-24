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
    if (!targetConfiguration_)
        return Messenger::error("No configuration target set for module '{}'.\n", uniqueName());

    // Get options
    Messenger::print("Benchmark: Test timings will be averaged over {} {}.\n", nRepeats_, nRepeats_ == 1 ? "run" : "runs");
    Messenger::print("Benchmark: Test timings {} be saved to disk.\n", save_ ? "will" : "will not");
    Messenger::print("\n");

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());
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
            targetConfiguration_->generate(procPool, dissolve.pairPotentialRange());
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "Generator"),
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
            rdfModule.keywords().set("Configuration", targetConfiguration_);
            targetConfiguration_->incrementContentsVersion();
            srand(dissolve.seed());

            // Run the Module calculation
            bool upToDate;
            Timer timer;
            Messenger::mute();
            rdfModule.calculateGR(dissolve.processingModuleData(), procPool, targetConfiguration_, RDFModule::CellsMethod,
                                  targetConfiguration_->box()->inscribedSphereRadius(), 0.05, upToDate);
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "RDFCells"),
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
            rdfModule.keywords().set("Configuration", targetConfiguration_);
            targetConfiguration_->incrementContentsVersion();
            srand(dissolve.seed());

            // Run the Module calculation
            bool upToDate;
            Timer timer;
            Messenger::mute();
            rdfModule.calculateGR(dissolve.processingModuleData(), procPool, targetConfiguration_, RDFModule::SimpleMethod,
                                  targetConfiguration_->box()->inscribedSphereRadius(), 0.05, upToDate);
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "RDFSimple"),
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
            EnergyModule::intraMolecularEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "IntraEnergy"),
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
            EnergyModule::interAtomicEnergy(procPool, targetConfiguration_, dissolve.potentialMap());
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "InterEnergy"),
                          "Interatomic energy", timing, save_);
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
            RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells(), procPool,
                                            strategy);

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
                    auto mol = targetConfiguration_->molecule(molId);
            }
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", uniqueName(), targetConfiguration_->niceName(), "RegionalDist"),
                          "Distributor (regional)", timing, save_);
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
