/*
	*** Neutron SQ Module - Options
	*** src/modules/neutronsq/options.cpp
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

#include "modules/neutronsq/neutronsq.h"
#include "module/keywordtypes.h"
#include "main/dissolve.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "genericitems/listhelper.h"

// Return enum option info for NormalisationType
EnumOptions<NeutronSQModule::NormalisationType> NeutronSQModule::normalisationTypes()
{
	static EnumOptionsList NormalisationTypeOptions = EnumOptionsList() << 
		EnumOption(NeutronSQModule::NoNormalisation,			"None") << 
		EnumOption(NeutronSQModule::AverageOfSquaresNormalisation,	"AverageSquared") << 
		EnumOption(NeutronSQModule::SquareOfAverageNormalisation,	"SquaredAverage");

	static EnumOptions<NeutronSQModule::NormalisationType> options("NormalisationType", NormalisationTypeOptions, NeutronSQModule::NoNormalisation);

	return options;
}

// Set up keywords for Module
void NeutronSQModule::setUpKeywords()
{
	frequency_ = 5;

	// Q range
	ModuleKeywordGroup* group = addKeywordGroup("Q Range");
	group->add(new DoubleModuleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
	group->add(new DoubleModuleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q) (and limit at which reference data will be truncated)");
	group->add(new DoubleModuleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
	group->add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "QBroadening", "Broadening function to apply when calculating S(Q)");
	group->add(new WindowFunctionModuleKeyword(WindowFunction(WindowFunction::Lorch0Window)), "WindowFunction", "Window function to apply when Fourier-transforming g(r) to S(Q)");

	// Neutron Isotopes
	group = addKeywordGroup("Neutron Isotopes");
	group->add(new AtomTypeSelectionModuleKeyword(exchangeableTypes_, targetConfigurations_), "Exchangeable", "Specify AtomTypes that are exchangeable", "<AtomType> [AtomType...]");
	group->add(new IsotopologueListModuleKeyword(isotopologues_), "Isotopologue", "Set Isotopologue (and its population) to use for a particular Species in a given Configuration");
	group->add(new EnumOptionsModuleKeyword(NeutronSQModule::normalisationTypes() = NeutronSQModule::NoNormalisation), "Normalisation", "Normalisation to apply to total weighted F(Q)");

	// Bragg Scattering
	group = addKeywordGroup("Bragg Scattering");
	group->add(new BoolModuleKeyword(false), "IncludeBragg", "Include Bragg scattering (if reflection data are present in the Configuration)");
	group->add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "BraggQBroadening", "Broadening function to apply, on top of any QBroadening, to Bragg scattering");

	// Reference Data
	group = addKeywordGroup("Reference Data");
	group->add(new FileAndFormatModuleKeyword(referenceFQ_), "Reference", "F(Q) reference data", "<format> <filename>");
	group->add(new EnumOptionsModuleKeyword(NeutronSQModule::normalisationTypes() = NeutronSQModule::NoNormalisation), "ReferenceNormalisation", "Normalisation to remove from reference data before use");
	group->add(new DoubleModuleKeyword(-1.0, -1.0), "ReferenceRemoveAverage", "Q value at which to form average level to be subtracted from reference data before use (-1 for no subtraction)");
	group->add(new BoolModuleKeyword(false), "ReferenceIgnoreFirst", "Ignore the first point in the supplied reference data");

	// Export
	group = addKeywordGroup("Export");
	group->add(new BoolModuleKeyword(false), "SaveReferenceData", "Whether to save the reference data and its Fourier transform", "<True|False>");
	group->add(new BoolModuleKeyword(false), "SaveUnweighted", "Whether to save unweighted totals / partials to disk after calculation", "<True|False>");
	group->add(new BoolModuleKeyword(false), "SaveWeighted", "Whether to save weighted totals / partials to disk after calculation", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int NeutronSQModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}

