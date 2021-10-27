// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "math/averaging.h"
#include "math/function1d.h"
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
     * Control
     */
    private:
    // Number of historical partial sets to combine into final partials
    int averagingLength_{1};
    // Weighting scheme to use when averaging partials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Broadening function to apply to Bragg S(Q)
    Functions::Function1DWrapper braggQBroadening_{Functions::Function1D::GaussianC2, {0.0, 0.02}};
    // Broadening function to apply to S(Q)
    Functions::Function1DWrapper qBroadening_;
    // Step size in Q for S(Q) calculation
    double qDelta_{0.05};
    // Maximum Q for calculated S(Q)
    double qMax_{30.0};
    // Minimum Q for calculated S(Q)
    double qMin_{0.01};
    // Whether to save partials to disk after calculation
    bool save_{false};
    // Window function to use when Fourier-transforming reference S(Q) to g(r))
    WindowFunction::Form windowFunction_{WindowFunction::Form::None};

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
                                      Functions::Function1DWrapper broadening);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
