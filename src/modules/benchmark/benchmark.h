// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
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
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions
     */
    private:
    // Print timing information, assessing it against last value in existing timings (if found)
    void printTimingResult(std::string_view testFile, std::string_view testDescription, const SampledDouble &timing,
                           bool storeNewTiming);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
