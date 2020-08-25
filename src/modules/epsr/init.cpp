/*
    *** EPSR Module - Initialisation
    *** src/modules/epsr/init.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "keywords/types.h"
#include "modules/epsr/epsr.h"

// Return enum option info for ExpansionFunctionType
EnumOptions<EPSRModule::ExpansionFunctionType> EPSRModule::expansionFunctionTypes()
{
    static EnumOptionsList ExpansionFunctionTypeOptions = EnumOptionsList()
                                                          << EnumOption(EPSRModule::PoissonExpansionFunction, "Poisson")
                                                          << EnumOption(EPSRModule::GaussianExpansionFunction, "Gaussian");

    static EnumOptions<EPSRModule::ExpansionFunctionType> options("ExpansionFunctionType", ExpansionFunctionTypeOptions,
                                                                  EPSRModule::PoissonExpansionFunction);

    return options;
}

// Perform any necessary initialisation for the Module
void EPSRModule::initialise()
{
    groupedTargets_.addAllowedModuleType("NeutronSQ");

    // Calculation
    keywords_.add("Calculation", new BoolKeyword(true), "OnlyWhenEnergyStable",
                  "Assesses the energy of all involved Configurations, refining the potential only when all their total "
                  "energies are stable");
    keywords_.add("Calculation", new DoubleKeyword(3.0, -1.0), "EReq",
                  "Limit of magnitude of additional potential for any one pair potential");
    keywords_.add("Calculation", new DoubleKeyword(0.8, 0.0, 1.0), "Feedback", "Confidence factor");
    keywords_.add("Calculation", new BoolKeyword(true), "ModifyPotential",
                  "Whether to apply generated perturbations to interatomic potentials");
    keywords_.add("Calculation", new ModuleGroupsKeyword(groupedTargets_), "Target",
                  "Add specified Module (and it's Reference data) as a refinement target", "<ModuleName> [GroupName]");
    keywords_.add("Calculation", new DoubleKeyword(30.0, -1.0), "QMax",
                  "Maximum Q value over which to generate potentials from total scattering data");
    keywords_.add("Calculation", new DoubleKeyword(0.5, -1.0), "QMin",
                  "Minimum Q value over which to generate potentials from total scattering data");
    keywords_.add("Calculation", new DoubleKeyword(1.0, 0.0, 10.0), "Weighting",
                  "Factor used when adding fluctuation coefficients to pair potentials");

    // Expansion Function
    keywords_.add("Expansion Function",
                  new EnumOptionsKeyword<EPSRModule::ExpansionFunctionType>(EPSRModule::expansionFunctionTypes() =
                                                                                EPSRModule::PoissonExpansionFunction),
                  "ExpansionFunction", "Form of expansion function to use when fitting difference data");
    keywords_.add("Expansion Function", new DoubleKeyword(0.1, 0.001, 1.0), "GSigma1",
                  "Width for Gaussian function in reciprocal space");
    keywords_.add("Expansion Function", new DoubleKeyword(0.2, 0.001, 1.0), "GSigma2",
                  "Width for Gaussian function in real space");
    keywords_.add("Expansion Function", new IntegerKeyword(-1, -1), "NCoeffP",
                  "Number of coefficients used to define the empirical potential (-1 for automatic)");
    keywords_.add("Expansion Function", new IntegerKeyword(1000, 0), "NPItSs", "Number of steps for refining the potential");
    keywords_.add("Expansion Function", new StringKeyword(""), "PCofFile",
                  "EPSR pcof file from which to read starting coefficients from");
    keywords_.add("Expansion Function", new DoubleKeyword(0.01, 0.001, 1.0), "PSigma1",
                  "Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)");
    keywords_.add("Expansion Function", new DoubleKeyword(0.01, 0.001, 1.0), "PSigma2",
                  "Width for Poisson functions in real space");
    keywords_.add("Expansion Function", new DoubleKeyword(-1.0), "RMaxPT",
                  "Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)");
    keywords_.add("Expansion Function", new DoubleKeyword(-1.0), "RMinPT",
                  "Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)");

    // Test
    keywords_.add("Test", new BoolKeyword(false), "Test", "Test against supplied reference data", "<True|False>");
    keywords_.add("Test", new Data1DStoreKeyword(testData_), "TestReference", "Specify test reference data",
                  "<target> <fileformat> <filename> [x=1] [y=2]");
    keywords_.add("Test", new DoubleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%error) above which test fails",
                  "<threshold[0.1]>");
    keywords_.add("Test", new BoolKeyword(false), "OverwritePotentials",
                  "Overwrite potentials each time rather than summing them", "<True|False>");

    // Export
    keywords_.add("Export", new BoolKeyword(false), "SaveDifferenceFunctions", "Whether to save difference function and fit",
                  "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveEmpiricalPotentials", "Whether to save empirical potentials",
                  "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SaveEstimatedPartials", "Whether to save estimated partials",
                  "<True|False>");
    keywords_.add("Export", new BoolKeyword(false), "SavePCof", "Whether to save potential coefficients", "<True|False>");
}
