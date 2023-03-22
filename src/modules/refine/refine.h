// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/data1dstore.h"
#include "classes/scatteringmatrix.h"
#include "math/data1d.h"
#include "module/groups.h"
#include "module/module.h"
#include "templates/array3d.h"

// Forward Declarations
class AtomType;
class PartialSet;

// Refine Module
class RefineModule : public Module
{
    public:
    RefineModule();
    ~RefineModule() override = default;

    /*
     * Definition
     */
    public:
    // Expansion Function Type Enum
    enum ExpansionFunctionType
    {
        PoissonExpansionFunction,  /* Fit difference functions using Poisson (power exponential) functions */
        GaussianExpansionFunction, /* Fit difference functions using Gaussian functions */
        nExpansionFunctionTypes
    };
    // Return enum option info for ExpansionFunctionType
    static EnumOptions<RefineModule::ExpansionFunctionType> expansionFunctionTypes();

    private:
    // Limit of magnitude of additional potential for any one pair potential
    double eReq_{3.0};
    // Expansion function type to use for potential fits
    RefineModule::ExpansionFunctionType expansionFunction_{RefineModule::PoissonExpansionFunction};
    // Confidence factor
    double feedback_{0.9};
    // Scattering matrix
    ScatteringMatrix scatteringMatrix_;
    // Maximum Q value over which to generate potentials from total scattering data
    double qMax_{30.0};
    // Minimum Q value over which to generate potentials from total scattering data
    double qMin_{0.5};
    // Factor used when adding fluctuation coefficients to pair potentials
    double weighting_{1.0};
    // Width for Gaussian function in reciprocal space
    double gSigma1_{0.1};
    // Width for Gaussian function in real space
    double gSigma2_{0.2};
    // Frequency at which to apply generated perturbations to interatomic potentials
    std::optional<int> modifyPotential_{1};
    // Number of coefficients used to define the empirical potential
    std::optional<int> nCoeffP_;
    // Number of steps for refining the potential
    std::optional<int> nPItSs_{1000};
    // Overwrite potentials each time rather than summing them
    bool overwritePotentials_{false};
    // Width for Poisson functions in reciprocal space (N.B. this is psigma2 in Refine)
    double pSigma1_{0.01};
    // Width for Poisson functions in real space
    double pSigma2_{0.01};
    // Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)
    std::optional<double> rMaxPT_;
    // Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)
    std::optional<double> rMinPT_;
    // Degree of smoothing to apply to fluctuation coefficients before summation into potential
    std::optional<int> fluctuationSmoothing_;
    // Whether to save difference function and fit
    bool saveDifferenceFunctions_{false};
    // Whether to save empirical potentials
    bool saveEmpiricalPotentials_{false};
    // Whether to save estimated partials
    bool saveEstimatedPartials_{false};
    // Whether to save potential coefficients
    bool savePotentialCoefficients_{false};
    // Whether to save simulated F(r) (Fourier transform of calculated F(Q))
    bool saveSimulatedFR_{false};
    // Target Modules containing data to refine against
    std::vector<Module *> targets_;
    // Test against supplied reference data
    bool test_{false};
    // Test absolute EP energy values
    std::vector<std::pair<std::string, double>> testAbsEnergyEP_;
    // Test threshold for absolute EP energies
    double testAbsEnergyEPThreshold_{1.0e-4};
    // Test reference data
    Data1DStore testReferenceData_;
    // Test threshold (%error) above which test fails
    double testThreshold_{0.1};

    public:
    // Return list of target Modules / data for refinement
    const std::vector<Module *> &targets() const;

    /*
     * Functions
     */
    private:
    // Target Configuration (determined from target modules)
    Configuration *targetConfiguration_{nullptr};

    private:
    // Create / update delta S(Q) information
    void updateDeltaSQ(GenericList &processingData,
                       OptionalReferenceWrapper<const Array2D<Data1D>> optCalculatedSQ = std::nullopt,
                       OptionalReferenceWrapper<const Array2D<Data1D>> optEstimatedSQ = std::nullopt);

    public:
    // Truncate the supplied data
    void truncate(Data1D &data, double rMin, double rMax);

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
