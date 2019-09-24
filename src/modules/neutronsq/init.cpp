/*
	*** Neutron SQ Module - Initialisation
	*** src/modules/neutronsq/init.cpp
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
#include "keywords/types.h"
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

// Perform any necessary initialisation for the Module
void NeutronSQModule::initialise()
{
	// Calculation
	keywords_.add("Calculation", new DoubleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
	keywords_.add("Calculation", new DoubleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q) (and limit at which reference data will be truncated)");
	keywords_.add("Calculation", new DoubleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
	keywords_.add("Calculation", new BroadeningFunctionKeyword(BroadeningFunction()), "QBroadening", "Broadening function to apply when calculating S(Q)");
	keywords_.add("Calculation", new WindowFunctionKeyword(WindowFunction(WindowFunction::Lorch0Window)), "WindowFunction", "Window function to apply when Fourier-transforming g(r) to S(Q)");

	// Neutron Isotopes
	keywords_.add("Neutron Isotopes", new AtomTypeSelectionKeyword(exchangeableTypes_, targetConfigurations_), "Exchangeable", "Specify AtomTypes that are exchangeable", "<AtomType> [AtomType...]");
	keywords_.add("Neutron Isotopes", new IsotopologueReferenceListKeyword(isotopologues_, targetConfigurations()), "Isotopologue", "Set Isotopologue (and its population) to use for a particular Species in a given Configuration");
	keywords_.add("Neutron Isotopes", new EnumOptionsKeyword<NeutronSQModule::NormalisationType>(NeutronSQModule::normalisationTypes() = NeutronSQModule::NoNormalisation), "Normalisation", "Normalisation to apply to total weighted F(Q)");

	// Bragg Scattering
	keywords_.add("Bragg Scattering", new BoolKeyword(false), "IncludeBragg", "Include Bragg scattering (if reflection data are present in the Configuration)");
	keywords_.add("Bragg Scattering", new BroadeningFunctionKeyword(BroadeningFunction()), "BraggQBroadening", "Broadening function to apply, on top of any QBroadening, to Bragg scattering");

	// Reference Data
	keywords_.add("Reference Data", new FileAndFormatKeyword(referenceFQ_), "Reference", "F(Q) reference data", "<format> <filename>");
	keywords_.add("Reference Data", new EnumOptionsKeyword<NeutronSQModule::NormalisationType>(NeutronSQModule::normalisationTypes() = NeutronSQModule::NoNormalisation), "ReferenceNormalisation", "Normalisation to remove from reference data before use");
	keywords_.add("Reference Data", new BoolKeyword(false), "ReferenceIgnoreFirst", "Ignore the first point in the supplied reference data");

	// Export
	keywords_.add("Export", new BoolKeyword(false), "SaveReference", "Whether to save the reference data and its Fourier transform", "<True|False>");
	keywords_.add("Export", new BoolKeyword(false), "SaveUnweighted", "Whether to save unweighted totals / partials to disk after calculation", "<True|False>");
	keywords_.add("Export", new BoolKeyword(false), "SaveWeighted", "Whether to save weighted totals / partials to disk after calculation", "<True|False>");
}
