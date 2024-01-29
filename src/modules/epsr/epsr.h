// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/scatteringMatrix.h"
#include "math/data1D.h"
#include "math/range.h"
#include "module/groups.h"
#include "module/module.h"
#include "templates/array3D.h"
#include <tuple>

// Forward Declarations
class AtomType;
class PartialSet;

// EPSR Module
class EPSRModule : public Module
{
    public:
    EPSRModule();
    ~EPSRModule() override = default;

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
    static EnumOptions<EPSRModule::ExpansionFunctionType> expansionFunctionTypes();

    private:
    // Limit of magnitude of additional potential for any one pair potential
    double eReq_{3.0};
    // Expansion function type to use for potential fits
    EPSRModule::ExpansionFunctionType expansionFunction_{EPSRModule::PoissonExpansionFunction};
    // Confidence factor
    double feedback_{0.9};
    // Scattering matrix
    ScatteringMatrix scatteringMatrix_;
    // EPSR 'inpa' file from which to read deltaFQ fit coefficients from
    std::string inpaFilename_;
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
    // Vector storing atom pairs and associated potentials
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>, Data1D>> empiricalPotentials_;
    // Frequency at which to apply generated perturbations to interatomic potentials
    std::optional<int> modifyPotential_{1};
    // Whether to apply this module's generated potentials to the global pair potentials
    bool applyPotentials_{true};
    // Number of coefficients used to define the empirical potential
    std::optional<int> nCoeffP_;
    // Number of steps for refining the potential
    std::optional<int> nPItSs_{1000};
    // Overwrite potentials each time rather than summing them
    bool overwritePotentials_{false};
    // EPSR pcof file from which to read starting coefficients from
    std::string pCofFilename_;
    // Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)
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
    // Weightings for targets (if not 1.0)
    std::vector<std::pair<Module *, double>> targetWeights_;
    // Ranges to calculate rFactor over
    std::vector<Range> ranges_;

    public:
    // Return list of target Modules / data for refinement
    const std::vector<Module *> &targets() const;
    // Return current scattering matrix
    const ScatteringMatrix &scatteringMatrix() const;
    // Set whether to apply this module's generated potentials to the global pair potentials
    void setApplyPotentials(bool b);

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
    // Create / retrieve arrays for storage of empirical potential coefficients
    Array2D<std::vector<double>> &potentialCoefficients(GenericList &moduleData, const int nAtomTypes,
                                                        std::optional<int> ncoeffp = std::nullopt);
    // Generate empirical potentials from current coefficients
    bool generateEmpiricalPotentials(Dissolve &dissolve, double rho, std::optional<int> ncoeffp, double rminpt, double rmaxpt,
                                     double sigma1, double sigma2);
    // Generate and return single empirical potential function
    Data1D generateEmpiricalPotentialFunction(Dissolve &dissolve, int i, int j, int n);
    // Calculate absolute energy of empirical potentials
    double absEnergyEP(GenericList &moduleData);
    // Truncate the supplied data
    void truncate(Data1D &data, double rMin, double rMax);
    // return vector of emirical potentials
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>, Data1D>> empiricalPotentials();

    /*
     * EPSR File I/O
     */
    public:
    // PCof File Keywords
    enum EPSRPCofKeyword
    {
        AddPotTypePCofKeyword, /* addpottype - Additional potential type: Gaussian or modmorse. [Gaussian] */
        ExpecFPCofKeyword,     /* expecf - Additional potential type: Gaussian or modmorse. [Gaussian] */
        GaussianPCofKeyword,   /* gaussian - Select T for Gaussian representation of EP. Otherwise Poisson. [F] */
        NCoeffPPCofKeyword,    /* ncoeffp - Number of coefficients used to define the EP. */
        NPItSSPCofKeyword,     /* npitss - Number of steps for refining the potential. */
        PAcceptPCofKeyword,    /* paccept - Acceptance factor for potential refinement. [0.0005] */
        PDMaxPCofKeyword,      /* pdmax - Maximum distance of Empirical Potential. */
        PDStepPCofKeyword,     /* pdstep - Spacing between coefficients in r. */
        PowerPCofKeyword,      /* power - Repulsive power in Lennard-Jones potential. [12] */
        PSigma2PCofKeyword,    /* psigma2 - Width for empirical potential functions. [0.01] */
        QuitPCofKeyword,       /* q - Signals the end of the pcof keyword section */
        RBroadPCofKeyword,     /* rbroad - Controls potential decay. [0.0] */
        RChargePCofKeyword,    /* rcharge - Calculates energy due to molecular polarisation. [0.0] */
        RefPotFacPCofKeyword,  /* refpotfac - Factor to apply to reference potential. [1.0] */
        RepPotTypePCofKeyword, /* reppottype - Repulsive potential type: exponential or harmonic. [exponential] */
        RMaxPtPCofKeyword,     /* rmaxpt - Radius at which potential truncation goes to 0.0. */
        RMinFacPCofKeyword,    /* rminfac - Factor to set the minimum separation between pairs. [0.0] */
        RMinPtPCofKeyword,     /* rminpt - Radius at which potential truncation begins. */
        ROverlapPCofKeyword,   /* roverlap - Minimum allowed intermolecular separation between two atoms. */
        nEPSRPCofKeywords
    };
    // Return enum options for EPSRPCofKeyword
    static EnumOptions<EPSRModule::EPSRPCofKeyword> epsrPCofKeywords();

    public:
    // Read data from supplied pcof file
    bool readPCof(Dissolve &dissolve, const ProcessPool &procPool, std::string_view filename);
    // Read Poisson coefficients from 'inpa' file
    bool readFitCoefficients(Dissolve &dissolve, const ProcessPool &procPool, std::string_view inpaFilename);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
