// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/sysFunc.h"
#include "classes/box.h"
#include "classes/regionalDistributor.h"
#include "io/export/data1D.h"
#include "io/import/data1D.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/benchmark/benchmark.h"
#include "modules/energy/energy.h"
#include "modules/gr/gr.h"

// Run main processing
Module::ExecutionResult BenchmarkModule::process(ModuleContext &moduleContext)
{
    // Get options
    Messenger::print("Benchmark: Test timings will be averaged over {} {}.\n", nRepeats_, nRepeats_ == 1 ? "run" : "runs");
    Messenger::print("Benchmark: Test timings {} be saved to disk.\n", save_ ? "will" : "will not");
    Messenger::print("\n");

    ProcessPool::DivisionStrategy strategy = moduleContext.processPool().bestStrategy();

    /*
     * Configuration Generation
     */
    if (testGenerator_)
    {
        SampledDouble timing;
        for (auto n = 0; n < nRepeats_; ++n)
        {
            Timer timer;
            Messenger::mute();
            targetConfiguration_->generate({moduleContext.processPool(), moduleContext.dissolve()});
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "Generator"),
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
            GRModule rdfModule;
            rdfModule.keywords().set("Configuration", targetConfiguration_);
            targetConfiguration_->incrementContentsVersion();

            // Run the Module calculation
            bool upToDate;
            Timer timer;
            Messenger::mute();
            rdfModule.calculateGR(moduleContext.dissolve().processingModuleData(), moduleContext.processPool(),
                                  targetConfiguration_, GRModule::CellsMethod,
                                  targetConfiguration_->box()->inscribedSphereRadius(), 0.05, upToDate);
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "RDFCells"),
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
            GRModule rdfModule;
            rdfModule.keywords().set("Configuration", targetConfiguration_);
            targetConfiguration_->incrementContentsVersion();

            // Run the Module calculation
            bool upToDate;
            Timer timer;
            Messenger::mute();
            rdfModule.calculateGR(moduleContext.dissolve().processingModuleData(), moduleContext.processPool(),
                                  targetConfiguration_, GRModule::SimpleMethod,
                                  targetConfiguration_->box()->inscribedSphereRadius(), 0.05, upToDate);
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "RDFSimple"),
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
            EnergyModule::intraMolecularEnergy(moduleContext.processPool(), targetConfiguration_,
                                               moduleContext.dissolve().potentialMap());
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "IntraEnergy"),
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
            EnergyModule::pairPotentialEnergy(moduleContext.processPool(), targetConfiguration_,
                                              moduleContext.dissolve().potentialMap());
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "InterEnergy"),
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
            RegionalDistributor distributor(targetConfiguration_->nMolecules(), targetConfiguration_->cells(),
                                            moduleContext.processPool(), strategy);

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
                }

                // Loop over target Molecules
                for (auto &molId : targetMolecules)
                    // Get Molecule index and pointer
                    auto mol = targetConfiguration_->molecule(molId);
            }
            Messenger::unMute();
            timing += timer.split();
        }
        printTimingResult(fmt::format("{}_{}_{}.txt", name(), targetConfiguration_->niceName(), "RegionalDist"),
                          "Distributor (regional)", timing, save_);
    }

    return ExecutionResult::Success;
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
