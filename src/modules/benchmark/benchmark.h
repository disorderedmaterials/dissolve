// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Forward Declarations
/* none */

// Benchmark Module
class BenchmarkModule : public Module
{
    public:
    BenchmarkModule();
    ~BenchmarkModule();

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const;
    // Return category for module
    std::string_view category() const;
    // Return brief description of module
    std::string_view brief() const;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool);

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
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve);
};
