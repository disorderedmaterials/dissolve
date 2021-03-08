// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "math/broadeningfunction.h"
#include "math/windowfunction.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;

// SQ Module
class SQModule : public Module
{
    public:
    SQModule();
    ~SQModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Initialisation
     */
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Members / Functions
     */
    private:
    // Test data
    Data1DStore testData_;

    public:
    // Calculate unweighted S(Q) from unweighted g(r)
    static bool calculateUnweightedSQ(ProcessPool &procPool, const PartialSet &unweightedgr, PartialSet &unweightedsq,
                                      double qMin, double qDelta, double qMax, double rho, const WindowFunction &windowFunction,
                                      const BroadeningFunction &broadening);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
