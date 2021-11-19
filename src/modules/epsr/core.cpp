// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/bool.h"
#include "keywords/data1dstore.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/modulevector.h"
#include "keywords/stdstring.h"
#include "modules/epsr/epsr.h"

EPSRModule::EPSRModule() : Module()
{
    // Initialise Module - set up keywords etc.
    // Targets
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Add specified Module (and it's Reference data) as a refinement target",
                                             targets_, std::vector<std::string>{"NeutronSQ", "XRaySQ"});

    // Control
    keywords_.add<BoolKeyword>(
        "Control", "OnlyWhenEnergyStable",
        "Assesses the energy of all involved Configurations, refining the potential only when all their total "
        "energies are stable",
        onlyWhenEnergyStable_);
    keywords_.add<DoubleKeyword>("Control", "EReq", "Limit of magnitude of additional potential for any one pair potential",
                                 eReq_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "Feedback", "Confidence factor", feedback_, 0.0, 1.0);
    keywords_.add<BoolKeyword>("Control", "ModifyPotential",
                               "Whether to apply generated perturbations to interatomic potentials", modifyPotential_);
    keywords_.add<DoubleKeyword>("Control", "QMax",
                                 "Maximum Q value over which to generate potentials from total scattering data", qMax_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "QMin",
                                 "Minimum Q value over which to generate potentials from total scattering data", qMin_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "Weighting", "Factor used when adding fluctuation coefficients to pair potentials",
                                 weighting_, 0.0, 100.0);

    // Expansion Function
    keywords_.add<EnumOptionsKeyword<EPSRModule::ExpansionFunctionType>>(
        "Expansion Function", "ExpansionFunction", "Form of expansion function to use when fitting difference data",
        expansionFunction_, EPSRModule::expansionFunctionTypes());
    keywords_.add<DoubleKeyword>("Expansion Function", "GSigma1", "Width for Gaussian function in reciprocal space", gSigma1_,
                                 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Expansion Function", "GSigma2", "Width for Gaussian function in real space", gSigma2_, 0.001,
                                 1.0);
    keywords_.add<IntegerKeyword>("Expansion Function", "NCoeffP",
                                  "Number of coefficients used to define the empirical potential (-1 for automatic)", nCoeffP_,
                                  -1);
    keywords_.add<IntegerKeyword>("Expansion Function", "NPItSs", "Number of steps for refining the potential", nPItSs_, 1);
    keywords_.add<StringKeyword>("Expansion Function", "PCofFile",
                                 "EPSR pcof file from which to read starting coefficients from", pCofFilename_);
    keywords_.add<DoubleKeyword>("Expansion Function", "PSigma1",
                                 "Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)", pSigma1_,
                                 0.001, 1.0);
    keywords_.add<DoubleKeyword>("Expansion Function", "PSigma2", "Width for Poisson functions in real space", pSigma2_, 0.001,
                                 1.0);
    keywords_.add<DoubleKeyword>("Expansion Function", "RMaxPT",
                                 "Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)",
                                 rMaxPT_, -1.0);
    keywords_.add<DoubleKeyword>("Expansion Function", "RMinPT",
                                 "Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)",
                                 rMinPT_, -1.0);

    // Test
    keywords_.add<BoolKeyword>("Test", "Test", "Test against supplied reference data", test_);
    keywords_.add<Data1DStoreKeyword>("Test", "TestReference", "Specify test reference data", testReferenceData_);
    keywords_.add<DoubleKeyword>("Test", "TestThreshold", "Test threshold (%error) above which test fails", testThreshold_,
                                 1.0e-5);
    keywords_.add<BoolKeyword>("Test", "OverwritePotentials", "Overwrite potentials each time rather than summing them",
                               overwritePotentials_);

    // Export
    keywords_.add<BoolKeyword>("Export", "SaveDifferenceFunctions", "Whether to save difference function and fit",
                               saveDifferenceFunctions_);
    keywords_.add<BoolKeyword>("Export", "SaveEmpiricalPotentials", "Whether to save empirical potentials",
                               saveEmpiricalPotentials_);
    keywords_.add<BoolKeyword>("Export", "SaveEstimatedPartials", "Whether to save estimated partials", saveEstimatedPartials_);
    keywords_.add<BoolKeyword>("Export", "SavePCof", "Whether to save potential coefficients", savePotentialCoefficients_);
    keywords_.add<BoolKeyword>("Export", "SaveSimulatedFR",
                               "Whether to save simulated F(r) (Fourier transform of calculated F(Q))", saveSimulatedFR_);
}

// Return enum option info for ExpansionFunctionType
EnumOptions<EPSRModule::ExpansionFunctionType> EPSRModule::expansionFunctionTypes()
{
    return EnumOptions<EPSRModule::ExpansionFunctionType>(
        "ExpansionFunctionType",
        {{EPSRModule::PoissonExpansionFunction, "Poisson"}, {EPSRModule::GaussianExpansionFunction, "Gaussian"}});
}

// Return list of target Modules / data for refinement
const std::vector<Module *> &EPSRModule::targets() const { return targets_; }
