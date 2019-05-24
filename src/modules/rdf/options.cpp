/*
	*** RDF Module - Options
	*** src/modules/rdf/options.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/rdf/rdf.h"
#include "module/keywordtypes.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Partial Calculation Method enum
const char* PartialsMethodKeywords[] = { "Auto", "Test", "Simple", "Cells" };

// Convert character string to PartialsMethod
RDFModule::PartialsMethod RDFModule::partialsMethod(const char* s)
{
	for (int n=0; n<nPartialsMethods; ++n) if (DissolveSys::sameString(s, PartialsMethodKeywords[n])) return (RDFModule::PartialsMethod) n;
	return RDFModule::nPartialsMethods;
}

// Return character string for PartialsMethod
const char* RDFModule::partialsMethod(RDFModule::PartialsMethod pm)
{
	return PartialsMethodKeywords[pm];
}

// Set up keywords for Module
void RDFModule::setUpKeywords()
{
	frequency_ = 5;

	// Calculation
	ModuleKeywordGroup* group = addKeywordGroup("Calculation");
	group->add(new IntegerModuleKeyword(5, 1), "Averaging", "Number of historical partial sets to combine into final partials", "<5>");
	group->add(new EnumStringModuleKeyword(Averaging::averagingSchemes() = Averaging::LinearAveraging), "AveragingScheme", "Weighting scheme to use when averaging partials", "<Linear>");
	group->add(new PairBroadeningFunctionModuleKeyword(PairBroadeningFunction()), "IntraBroadening", "Type of broadening to apply to intramolecular g(r)");
	group->add(new EnumStringModuleKeyword(RDFModule::AutoMethod, RDFModule::nPartialsMethods, PartialsMethodKeywords), "Method", "Calculation method for partial radial distribution functions");
	group->add(new IntegerModuleKeyword(0, 0, 100), "Smoothing", "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in the applied Spline smooth");

	// Test
	group = addKeywordGroup("Test");
	group->add(new BoolModuleKeyword(false), "InternalTest", "Perform internal check of calculated partials (relative to Test method)");
	group->add(new BoolModuleKeyword(false), "Test", "Test calculated total and partials against supplied reference data", "<True|False>");
	group->add(new Data1DStoreModuleKeyword(testData_), "TestReference", "Specify test reference data", "<target> <fileformat> <filename> [x=1] [y=2]");
	group->add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");

	// Export
	group = addKeywordGroup("Export");
	group->add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int RDFModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}

