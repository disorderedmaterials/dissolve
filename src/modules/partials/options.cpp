/*
	*** Partials Module - Options
	*** src/modules/partials/options.cpp
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

#include "modules/partials/partials.h"
#include "module/keywordtypes.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Partial Calculation Method enum
const char* PartialsMethodKeywords[] = { "Auto", "Test", "Simple", "Cells" };

// Convert character string to PartialsMethod
PartialsModule::PartialsMethod PartialsModule::partialsMethod(const char* s)
{
	for (int n=0; n<nPartialsMethods; ++n) if (DUQSys::sameString(s, PartialsMethodKeywords[n])) return (PartialsModule::PartialsMethod) n;
	return PartialsModule::nPartialsMethods;
}

// Return character string for PartialsMethod
const char* PartialsModule::partialsMethod(PartialsModule::PartialsMethod pm)
{
	return PartialsMethodKeywords[pm];
}

// Weighting Type enum
const char* WeightingTypeKeywords[] = { "None", "Neutron" };

// Convert character string to WeightingType
PartialsModule::WeightingType PartialsModule::weightingType(const char* s)
{
	for (int n=0; n<nWeightingTypes; ++n) if (DUQSys::sameString(s, WeightingTypeKeywords[n])) return (PartialsModule::WeightingType) n;
	return PartialsModule::nWeightingTypes;
}

// Return character string for WeightingType
const char* PartialsModule::weightingType(PartialsModule::WeightingType wt)
{
	return WeightingTypeKeywords[wt];
}

// Normalisation Type enum
const char* NormalisationTypeKeywords[] = { "None", "AverageSquared", "SquaredAverage" };

// Convert character string to NormalisationType
PartialsModule::NormalisationType PartialsModule::normalisationType(const char* s)
{
	for (int n=0; n<PartialsModule::nNormalisationTypes; ++n) if (DUQSys::sameString(s, NormalisationTypeKeywords[n])) return (PartialsModule::NormalisationType) n;
	return PartialsModule::nNormalisationTypes;
}

// Return character string for NormalisationType
const char* PartialsModule::normalisationType(PartialsModule::NormalisationType nt)
{
	return NormalisationTypeKeywords[nt];
}

// Averaging scheme enum
const char* AveragingSchemeKeywords[] = { "Simple", "Exponential" };

// Convert character string to AveragingScheme
PartialsModule::AveragingScheme PartialsModule::averagingScheme(const char* s)
{
	for (int n=0; n<nAveragingSchemes; ++n) if (DUQSys::sameString(s, AveragingSchemeKeywords[n])) return (PartialsModule::AveragingScheme) n;
	return PartialsModule::nAveragingSchemes;
}

// Return character string for AveragingScheme
const char* PartialsModule::averagingScheme(PartialsModule::AveragingScheme as)
{
	return AveragingSchemeKeywords[as];
}

// Set up keywords for Module
void PartialsModule::setUpKeywords()
{
	frequency_ = 5;
	keywords_.add(new BoolModuleKeyword(false), "AllIntra", "Consider all intramolecular pairs in intra partials", "<True|False*>");
	keywords_.add(new IntegerModuleKeyword(5, 0), "Averaging", "Number of historical partial sets to combine into final partials", "<N[5]>");
	keywords_.add(new ComplexModuleKeyword(2,2), "ConfigurationWeight", "Sets the relative weight of the specified Configuration in construction of the partials", "<Configuration Name> <weight>");
	keywords_.add(new ComplexModuleKeyword(1), "Exchangeable", "Define an exchangeable group of atoms", "<AtomType> [AtomType ...]");
	keywords_.add(new CharStringModuleKeyword("Exponential", PartialsModule::nAveragingSchemes, AveragingSchemeKeywords), "AveragingScheme", "Weighting scheme to use when averaging partials", "<scheme[Exponential]>");
	keywords_.add(new BoolModuleKeyword(false), "Bragg", "Enable calculation of Bragg scattering", "<True|False*>");
	keywords_.add(new DoubleModuleKeyword(0.001), "BraggQResolution", "Binwidth in Q to use when calculating Bragg peaks");
	keywords_.add(new BoolModuleKeyword(false), "InternalTest", "Perform internal check of calculated partials (relative to Test method)");
	keywords_.add(new ComplexModuleKeyword(4,4), "Isotopologue", "Set Isotopologue (and its population) to use for a particular Species in a given Configuration");
	keywords_.add(new CharStringModuleKeyword("Auto", PartialsModule::nPartialsMethods, PartialsMethodKeywords), "Method", "Calculation method for partial radial distribution functions");
	keywords_.add(new CharStringModuleKeyword("None", PartialsModule::nNormalisationTypes, NormalisationTypeKeywords), "Normalisation", "Normalisation to apply to total weighted F(Q)");
	keywords_.add(new DoubleModuleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
	keywords_.add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "QBroadening", "Instrument broadening function to apply when calculating S(Q)");
	keywords_.add(new DoubleModuleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q)");
	keywords_.add(new DoubleModuleKeyword(0.0, 0.0), "QMin", "Minimum Q for calculated S(Q)");
	keywords_.add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
	keywords_.add(new IntegerModuleKeyword(0, 0, 100), "Smoothing", "Specifies the degree of smoothing 'n' to apply to calculated g(r), where 2n+1 controls the length in the applied Spline smooth");
	keywords_.add(new BoolModuleKeyword(false), "StructureFactor", "Determines whether S(Q) are to be calculated from F.T. of the g(r)", "<True|False>");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test calculated total and partials against supplied reference data", "<True|False>");
	keywords_.add(new ComplexModuleKeyword(2,4), "TestReference", "Specify g(r)/S(Q) test reference data", "<filename> <target> [xcol] [ycol]");
	keywords_.add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
	keywords_.add(new CharStringModuleKeyword("None", PartialsModule::nWeightingTypes, WeightingTypeKeywords), "Weights", "Weighting scheme to use for calculated partials (None,Neutron)");
	keywords_.add(new WindowFunctionModuleKeyword(WindowFunction()), "WindowFunction", "Window function to apply when Fourier-transforming g(r) to S(Q)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int PartialsModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
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

		GenericListHelper<double>::add(targetList, CharString("%s_Weight", targetCfg->niceName()), uniqueName()) = parser.argd(2);
	}
	else if (DUQSys::sameString(parser.argc(0), "Exchangeable"))
	{
		// Define an exchangeable group of atoms
		// Loop over all provided arguments (which are atom type names) and add them to our list
		AtomTypeList exchangeableAtoms;
		for (int n=1; n<parser.nArgs(); ++n)
		{
			AtomType* atomType = duq->findAtomType(parser.argc(n));
			if (!atomType)
			{
				Messenger::error("Unrecognised AtomType '%s' given in Exchangeable keyword.\n", parser.argc(n));
				return false;
			}
			exchangeableAtoms.add(atomType);
		}

		GenericListHelper<AtomTypeList>::add(targetList, "Exchangeable", uniqueName()) = exchangeableAtoms;
	}
	else if (DUQSys::sameString(parser.argc(0), "Isotopologue"))
	{
		// Essentially a shortcut for setting a variable in a target Configuration / Sample
		// Find target Configuration
		Configuration* targetCfg = duq->findConfiguration(parser.argc(1));
		if (!targetCfg)
		{
			Messenger::error("Error defining Isotopologue - no Configuration named '%s' exists.\n", parser.argc(1));
			return false;
		}

		// Raise an error if this Configuration is not targetted by the Module
		if (!isTargetConfiguration(targetCfg)) 
		{
			Messenger::error("Configuration '%s' is not targetted by the Module '%s'.\n", targetCfg->name(), name());
			return false;
		}

		// Find specified Species - must be present in the target Configuration
		Species* sp = duq->findSpecies(parser.argc(2));
		if (!sp)
		{
			Messenger::error("Error defining Isotopologue - no Species named '%s' exists.\n", parser.argc(2));
			return false;
		}

		if (!targetCfg->hasUsedSpecies(sp))
		{
			Messenger::error("Error defining Isotopologue - Species '%s' is not present in Configuration '%s'.\n", sp->name(), targetCfg->name());
			return false;
		}

		// Finally, locate isotopologue definition for species
		Isotopologue* tope = sp->findIsotopologue(parser.argc(3));
		if (!tope)
		{
			Messenger::error("Error defining Isotopologue - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(3), sp->name());
			return false;
		}

		// Ready - add a suitable variable to the Configuration
		CharString varName("Isotopologue/%s/%s", sp->name(), tope->name());
		GenericListHelper<double>::add(targetList, varName, uniqueName()) = parser.argd(4);
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

