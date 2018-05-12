/*
	*** Neutron SQ Module - Options
	*** src/modules/neutronsq/options.cpp
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

#include "modules/neutronsq/neutronsq.h"
#include "module/keywordtypes.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Normalisation Type enum
const char* NormalisationTypeKeywords[] = { "None", "AverageSquared", "SquaredAverage" };

// Convert character string to NormalisationType
NeutronSQModule::NormalisationType NeutronSQModule::normalisationType(const char* s)
{
	for (int n=0; n<NeutronSQModule::nNormalisationTypes; ++n) if (DUQSys::sameString(s, NormalisationTypeKeywords[n])) return (NeutronSQModule::NormalisationType) n;
	return NeutronSQModule::nNormalisationTypes;
}

// Return character string for NormalisationType
const char* NeutronSQModule::normalisationType(NeutronSQModule::NormalisationType nt)
{
	return NormalisationTypeKeywords[nt];
}

// Set up keywords for Module
void NeutronSQModule::setUpKeywords()
{
	frequency_ = 5;
	keywords_.add(new IsotopologueListModuleKeyword(isotopologues_), "Isotopologue", "Set Isotopologue (and its population) to use for a particular Species in a given Configuration");
	keywords_.add(new CharStringModuleKeyword("None", NeutronSQModule::nNormalisationTypes, NormalisationTypeKeywords), "Normalisation", "Normalisation to apply to total weighted F(Q)");
	keywords_.add(new DoubleModuleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
	keywords_.add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "QBroadening", "Instrument broadening function to apply when calculating S(Q)");
	keywords_.add(new DoubleModuleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q)");
	keywords_.add(new DoubleModuleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
	keywords_.add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test calculated total and partials against supplied reference data", "<True|False>");
	keywords_.add(new ComplexModuleKeyword(2,4), "TestReference", "Specify S(Q) test reference data", "<filename> <target> [xcol] [ycol]");
	keywords_.add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
	keywords_.add(new WindowFunctionModuleKeyword(WindowFunction(WindowFunction::Lorch0Window)), "WindowFunction", "Window function to apply when Fourier-transforming g(r) to S(Q)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int NeutronSQModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString(parser.argc(0), "TestReference"))
	{
		Messenger::print("Reading test reference S(Q) / F(Q) data...\n");

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

