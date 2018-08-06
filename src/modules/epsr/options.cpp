/*
	*** EPSR Module - Options
	*** src/modules/epsr/options.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/epsr/epsr.h"
#include "module/list.h"
#include "module/keywordtypes.h"
#include "base/lineparser.h"

// Expansion Function Type
const char* ExpansionFunctionTypeKeywords[] = { "Poisson", "Gaussian" };

// Convert text string to ExpansionFunctionType
EPSRModule::ExpansionFunctionType EPSRModule::expansionFunctionType(const char* s)
{
	for (int n=0; n<EPSRModule::nExpansionFunctionTypes; ++n) if (DissolveSys::sameString(s, ExpansionFunctionTypeKeywords[n])) return (EPSRModule::ExpansionFunctionType) n;
	return EPSRModule::nExpansionFunctionTypes;
}

// Convert ExpansionFunctionType to text string
const char* EPSRModule::expansionFunctionType(EPSRModule::ExpansionFunctionType eft)
{
	return ExpansionFunctionTypeKeywords[eft];
}

// Set up keywords for Module
void EPSRModule::setUpKeywords()
{
	keywords_.add(new DoubleModuleKeyword(3.0, -1.0), "EReq", "Limit of magnitude of additional potential for any one pair potential");
	keywords_.add(new CharStringModuleKeyword("Poisson", EPSRModule::nExpansionFunctionTypes, ExpansionFunctionTypeKeywords), "ExpansionFunction", "Form of expansion function to use when fitting difference data");
	keywords_.add(new DoubleModuleKeyword(0.9), "Feedback", "Confidence factor");
	keywords_.add(new DoubleModuleKeyword(0.1, 0.001, 1.0), "GSigma1", "Width for Gaussian function in reciprocal space");
	keywords_.add(new DoubleModuleKeyword(0.2, 0.001, 1.0), "GSigma2", "Width for Gaussian function in real space");
	keywords_.add(new BoolModuleKeyword(true), "ModifyPotential", "Whether to apply generated perturbations to interatomic potentials");
	keywords_.add(new IntegerModuleKeyword(-1, -1), "NCoeffP", "Number of coefficients user to define the empirical potential (-1 for automatic)");
	keywords_.add(new IntegerModuleKeyword(1000, 10), "NPItSs", "Number of steps for refining the potential");
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenEnergyStable", "Assesses the energy of all involved Configurations, refining the potential only when all their total energies are stable");
	keywords_.add(new CharStringModuleKeyword(""), "PCofFile", "EPSR pcof file from which to read starting coefficients from");
	keywords_.add(new DoubleModuleKeyword(0.1, 0.001, 10.0), "PCofRho", "Atomic number density with which to weight coefficients from pcof file");
	keywords_.add(new DoubleModuleKeyword(0.01, 0.001, 1.0), "PSigma1", "Width for Poisson functions in reciprocal space (N.B. this is psigma2 in EPSR)");
	keywords_.add(new DoubleModuleKeyword(0.01, 0.001, 1.0), "PSigma2", "Width for Poisson functions in real space");
	keywords_.add(new DoubleModuleKeyword(-1.0), "RMaxPT", "Radius at which potential truncation goes to zero (-1.0 to use pair potential maximum range)");
	keywords_.add(new DoubleModuleKeyword(-1.0), "RMinPT", "Radius at which potential truncation begins (-1.0 to set to 2.0 Angstroms under rmaxpt)");
	keywords_.add(new DoubleModuleKeyword(30.0, -1.0), "QMax", "Maximum Q value over which to generate potentials from total scattering data");
	keywords_.add(new DoubleModuleKeyword(0.5, -1.0), "QMin", "Minimum Q value over which to generate potentials from total scattering data");
	keywords_.add(new ComplexModuleKeyword(1,2), "Target", "Add specified Module (and it's Reference data) as a fitting target", "<ModuleName> [GroupName]");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test against supplied reference data", "<True|False>");
	keywords_.add(new XYDataStoreModuleKeyword(testData_), "TestReference", "Specify test reference data", "<filename> <target> [xcol] [ycol]");
	keywords_.add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
	keywords_.add(new DoubleModuleKeyword(1.0, 0.0, 10.0), "Weighting", "Fractional (maximal) amounts of generated perturbations to apply to pair potentials");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int EPSRModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	if (DissolveSys::sameString(parser.argc(0), "Target")) return addTarget(parser.argc(1), parser.hasArg(2) ? parser.argc(2) : "Default");
	else return -1;

	return true;
}
