// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/data1dstore.h"
#include "math/data1d.h"
#include "module/groups.h"
#include "module/module.h"
#include "templates/array3d.h"

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
        GaussianExpansionFunction, /* Fit difference functiuns using Gaussian functions */
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
    double feedback_{0.8};
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
    // Whether to apply generated perturbations to interatomic potentials
    bool modifyPotential_{true};
    // Number of coefficients used to define the empirical potential (-1 for automatic)
    std::optional<int> nCoeffP_;
    // Number of steps for refining the potential
    std::optional<int> nPItSs_{1000};
    // Whether to only modify potentials if configuration energy(s) are stable
    bool onlyWhenEnergyStable_{true};
    // Overwrite potentials each time rather than summing them
    bool overwritePotentials_{false};
    // EPSR pcof file from which to read starting coefficients from
    std::string pCofFilename_;
    // Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)
    double pSigma1_{0.01};
    // Width for Poisson functions in real space
    double pSigma2_{0.01};
    // Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)
    double rMaxPT_{-1.0};
    // Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)
    double rMinPT_{-1.0};
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
    // Create / retrieve arrays for storage of empirical potential coefficients
    Array2D<std::vector<double>> &potentialCoefficients(Dissolve &dissolve, const int nAtomTypes,
                                                        std::optional<int> ncoeffp = std::nullopt);
    // Generate empirical potentials from current coefficients
    bool generateEmpiricalPotentials(Dissolve &dissolve, EPSRModule::ExpansionFunctionType functionType, double rho,
                                     std::optional<int> ncoeffp, double rminpt, double rmaxpt, double sigma1, double sigma2);
    // Generate and return single empirical potential function
    Data1D generateEmpiricalPotentialFunction(Dissolve &dissolve, int i, int j, int n);
    // Calculate absolute energy of empirical potentials
    double absEnergyEP(Dissolve &dissolve);
    // Test absolute energy of empirical potentials
    bool testAbsEnergyEP(Dissolve &dissolve);
    // Truncate the supplied data
    void truncate(Data1D &data, double rMin, double rMax);

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
    bool readPCof(Dissolve &dissolve, ProcessPool &procPool, std::string_view filename);
    // Read Poisson coefficients from 'inpa' file
    bool readFitCoefficients(Dissolve &dissolve, ProcessPool &procPool, std::string_view inpaFilename);

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
