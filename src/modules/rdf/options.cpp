/*
	*** RDF Module - Options
	*** src/modules/rdf/options.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/rdf/rdf.h"
#include "module/keywordtypes.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Partial Calculation Method enum
const char* PartialsMethodKeywords[] = { "Auto", "Test", "Simple", "Cells" };

// Convert character string to PartialsMethod
RDFModule::PartialsMethod RDFModule::partialsMethod(const char* s)
{
	for (int n=0; n<nPartialsMethods; ++n) if (DUQSys::sameString(s, PartialsMethodKeywords[n])) return (RDFModule::PartialsMethod) n;
	return RDFModule::nPartialsMethods;
}

// Return character string for PartialsMethod
const char* RDFModule::partialsMethod(RDFModule::PartialsMethod pm)
{
	return PartialsMethodKeywords[pm];
}

// Averaging scheme enum
const char* AveragingSchemeKeywords[] = { "Simple", "Exponential" };

// Convert character string to AveragingScheme
RDFModule::AveragingScheme RDFModule::averagingScheme(const char* s)
{
	for (int n=0; n<nAveragingSchemes; ++n) if (DUQSys::sameString(s, AveragingSchemeKeywords[n])) return (RDFModule::AveragingScheme) n;
	return RDFModule::nAveragingSchemes;
}

// Return character string for AveragingScheme
const char* RDFModule::averagingScheme(RDFModule::AveragingScheme as)
{
	return AveragingSchemeKeywords[as];
}

// Set up keywords for Module
void RDFModule::setUpKeywords()
{
	frequency_ = 5;
	keywords_.add(new BoolModuleKeyword(false), "AllIntra", "Consider all intramolecular pairs in intra partials", "<True|False*>");
	keywords_.add(new IntegerModuleKeyword(5, 0), "Averaging", "Number of historical partial sets to combine into final partials", "<N[5]>");
	keywords_.add(new ComplexModuleKeyword(2,2), "ConfigurationWeight", "Sets the relative weight of the specified Configuration in construction of the partials", "<Configuration Name> <weight>");
	keywords_.add(new CharStringModuleKeyword("Simple", RDFModule::nAveragingSchemes, AveragingSchemeKeywords), "AveragingScheme", "Weighting scheme to use when averaging partials", "<scheme[Exponential]>");
	keywords_.add(new BoolModuleKeyword(false), "InternalTest", "Perform internal check of calculated partials (relative to Test method)");
	keywords_.add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "IntraBroadening", "Broadening function to apply to intramolecular g(r)");
	keywords_.add(new CharStringModuleKeyword("Auto", RDFModule::nPartialsMethods, PartialsMethodKeywords), "Method", "Calculation method for partial radial distribution functions");
	keywords_.add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
	keywords_.add(new IntegerModuleKeyword(0, 0, 100), "Smoothing", "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in the applied Spline smooth");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test calculated total and partials against supplied reference data", "<True|False>");
	keywords_.add(new ComplexModuleKeyword(2,4), "TestReference", "Specify g(r)/S(Q) test reference data", "<filename> <target> [xcol] [ycol]");
	keywords_.add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int RDFModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString(parser.argc(0), "ConfigurationWeight"))
	{
		// Sets the weight of a specified Configuration in construction of the partials
		// Find target Configuration
		Configuration* targetCfg = duq->findConfiguration(parser.argc(1));
		if (!targetCfg)
		{
			Messenger::error("Error setting Configuration weight - no Configuration named '%s' exists.\n", parser.argc(1));
			return false;
		}

		// Raise an error if this Configuration is not targetted by the Module
		if (!isTargetConfiguration(targetCfg)) 
		{
			Messenger::error("Configuration '%s' is not targetted by the Module '%s', so setting its weight is irrelevant.\n", targetCfg->name(), name());
			return false;
		}

		GenericListHelper<double>::add(targetList, CharString("Weight_%s", targetCfg->niceName()), uniqueName()) = parser.argd(2);
	}
	else if (DUQSys::sameString(parser.argc(0), "TestReference"))
	{
		Messenger::print("Reading test reference g(r) / G(r) / S(Q) / F(Q) data...\n");

		// Realise an XYData to store the reference data in
		XYData& data = GenericListHelper<XYData>::realise(targetList, CharString("TestReference%s", parser.argc(2)), uniqueName());

		// Fourth and fifth arguments are x and y columns respectively (defaulting to 0,1 if not given)
		int xcol = parser.hasArg(3) ? parser.argi(3)-1 : 0;
		int ycol = parser.hasArg(4) ? parser.argi(4)-1 : 1;

		LineParser fileParser(&duq->worldPool());
		if ((!fileParser.openInput(parser.argc(1))) || (!data.load(fileParser, xcol, ycol))) return false;
	}
	else return -1;

	return true;
}

