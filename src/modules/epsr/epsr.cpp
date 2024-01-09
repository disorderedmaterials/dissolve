// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/epsr/epsr.h"
#include "keywords/bool.h"
#include "keywords/double.h"
#include "keywords/moduleVector.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "keywords/rangeVector.h"
#include "keywords/stdString.h"
#include "keywords/weightedModuleVector.h"

EPSRModule::EPSRModule() : Module(ModuleTypes::EPSR)
{
    keywords_.addTarget<ModuleVectorKeyword>("Target", "Add specified Module (and it's Reference data) as a refinement target",
                                             targets_,
                                             std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ});
    keywords_.addTarget<WeightedModuleVectorKeyword>(
        "TargetWeight", "Set relative weighting for specific module targets as they enter into the scattering matrix",
        targetWeights_, std::vector<ModuleTypes::ModuleType>{ModuleTypes::NeutronSQ, ModuleTypes::XRaySQ});

    keywords_.setOrganisation("Options", "Control");
    keywords_.add<DoubleKeyword>("EReq", "Limit of magnitude of additional potential for any one pair potential", eReq_, 0.0);
    keywords_.add<DoubleKeyword>("Feedback", "Confidence factor", feedback_, 0.0, 1.0);
    keywords_.add<OptionalIntegerKeyword>("ModifyPotential",
                                          "Frequency at which to apply generated perturbations to interatomic potentials",
                                          modifyPotential_, 0, std::nullopt, 1, "Off");
    keywords_.add<DoubleKeyword>("QMin", "Minimum Q value over which to generate potentials from total scattering data", qMin_,
                                 0.0);
    keywords_.add<DoubleKeyword>("QMax", "Maximum Q value over which to generate potentials from total scattering data", qMax_,
                                 0.0);

    keywords_.setOrganisation("Advanced", "Control");
    keywords_.add<DoubleKeyword>("Weighting", "Factor used when adding fluctuation coefficients to pair potentials", weighting_,
                                 0.0, 100.0);

    keywords_.setOrganisation(
        "Advanced", "Additional R-Factors",
        "Specify additional Q-ranges over which to calculate R-factors in addition to that over the whole Q-range.");
    keywords_.add<RangeVectorKeyword>("RFactorRanges", "Ranges over which to calculate RFactors", ranges_);

    keywords_.setOrganisation("Advanced", "Expansion Function",
                              "Control the expansion function used for fitting delta functions.");
    keywords_.add<EnumOptionsKeyword<EPSRModule::ExpansionFunctionType>>(
        "ExpansionFunction", "Form of expansion function to use when fitting difference data", expansionFunction_,
        EPSRModule::expansionFunctionTypes());
    keywords_.add<DoubleKeyword>("GSigma1", "Width for Gaussian function in reciprocal space", gSigma1_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("GSigma2", "Width for Gaussian function in real space", gSigma2_, 0.001, 1.0);
    keywords_.add<OptionalIntegerKeyword>("NCoeffP", "Number of coefficients used to define the empirical potential", nCoeffP_,
                                          0, std::nullopt, 100, "Automatic");
    keywords_.add<OptionalIntegerKeyword>("NPItSs", "Number of iterations when refining fits to delta functions", nPItSs_, 0,
                                          std::nullopt, 100, "Off (No Fitting - CAUTION!)");
    keywords_.add<StringKeyword>("InpAFile", "EPSR inpa file from which to read starting coefficients from", inpaFilename_);
    keywords_.add<StringKeyword>("PCofFile", "EPSR pcof file from which to read empirical potential coefficients from",
                                 pCofFilename_);
    keywords_.add<DoubleKeyword>("PSigma1", "Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)",
                                 pSigma1_, 0.001, 1.0);
    keywords_.add<DoubleKeyword>("PSigma2", "Width for Poisson functions in real space", pSigma2_, 0.001, 1.0);
    keywords_.add<OptionalDoubleKeyword>("RMaxPT", "Radius at which potential truncation goes to zero", rMaxPT_, 0.0,
                                         std::nullopt, 1.0, "Use Cuttoff Range");
    keywords_.add<OptionalDoubleKeyword>("RMinPT",
                                         "Radius at which potential truncation begins (or Auto for 2 Angstroms under rmaxpt)",
                                         rMinPT_, 0.0, std::nullopt, 1.0, "Auto");
    keywords_.add<OptionalIntegerKeyword>("Smoothing",
                                          "Smoothing to apply to fluctuation coefficients before summation into potential",
                                          fluctuationSmoothing_, 0, std::nullopt, 1, "Off");

    keywords_.setOrganisation("Advanced", "Test");
    keywords_.add<BoolKeyword>("OverwritePotentials", "Overwrite potentials each time rather than summing them",
                               overwritePotentials_);

    keywords_.setOrganisation("Export");
    keywords_.add<BoolKeyword>("SaveDifferenceFunctions", "Whether to save difference function and fit",
                               saveDifferenceFunctions_);
    keywords_.add<BoolKeyword>("SaveEmpiricalPotentials", "Whether to save empirical potentials", saveEmpiricalPotentials_);
    keywords_.add<BoolKeyword>("SaveEstimatedPartials", "Whether to save estimated partials", saveEstimatedPartials_);
    keywords_.add<BoolKeyword>("SavePCof", "Whether to save potential coefficients", savePotentialCoefficients_);
    keywords_.add<BoolKeyword>("SaveSimulatedFR", "Whether to save simulated F(r) (Fourier transform of calculated F(Q))",
                               saveSimulatedFR_);
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

// Return current scattering matrix
const ScatteringMatrix &EPSRModule::scatteringMatrix() const { return scatteringMatrix_; }
