// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    /*
     * Generates interatomic potentials using methodology mimicking EPSR as closely as possible.
     */

    public:
    EPSRModule();
    ~EPSRModule() override = default;

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

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Functions
     */
    private:
    // Test datasets (if any)
    Data1DStore testData_;
    // Target Configuration (determined from target modules)
    Configuration *targetConfiguration_;

    private:
    // Create / update delta S(Q) information
    void updateDeltaSQ(GenericList &processingData,
                       OptionalReferenceWrapper<const Array2D<Data1D>> optCalculatedSQ = std::nullopt,
                       OptionalReferenceWrapper<const Array2D<Data1D>> optEstimatedSQ = std::nullopt);

    public:
    // Return list of target Modules / data for refinement
    const std::vector<Module *> &targets() const;
    // Create / retrieve arrays for storage of empirical potential coefficients
    Array2D<std::vector<double>> &potentialCoefficients(Dissolve &dissolve, const int nAtomTypes, const int ncoeffp = -1);
    // Generate empirical potentials from current coefficients
    bool generateEmpiricalPotentials(Dissolve &dissolve, EPSRModule::ExpansionFunctionType functionType, double rho,
                                     int ncoeffp, double rminpt, double rmaxpt, double sigma1, double sigma2);
    // Generate and return single empirical potential function
    Data1D generateEmpiricalPotentialFunction(Dissolve &dissolve, int i, int j, int n);
    // Calculate absolute energy of empirical potentials
    double absEnergyEP(Dissolve &dissolve);
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

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
