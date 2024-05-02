// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Benchmark Module
class BenchmarkModule : public Module
{
    public:
    BenchmarkModule();
    ~BenchmarkModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Number of times to run each benchmark in order to form average
    int nRepeats_{5};
    // Whether to save new timings to the restart file
    bool save_{true};
    // Whether to benchmark intramolecular energy calculation
    bool testIntraEnergy_{true};
    // Whether to benchmark interatomic energy calculation
    bool testInterEnergy_{true};
    // Whether to benchmark configuration generator
    bool testGenerator_{true};
    // Whether to benchmark the RDF cells method (to half-cell range)
    bool testRDFCells_{true};
    // Whether to benchmark the RDF simple method (to half-cell range)
    bool testRDFSimple_{false};
    // Whether to benchmark molecule distributors
    bool testDistributors_{true};

    /*
     * Functions
     */
    private:
    // Print timing information, assessing it against last value in existing timings (if found)
    void printTimingResult(std::string_view testFile, std::string_view testDescription, const SampledDouble &timing,
                           bool storeNewTiming);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
