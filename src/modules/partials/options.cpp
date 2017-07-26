/*
	*** Partials Module - Options
	*** src/modules/partials/options.cpp
	Copyright T. Youngs 2012-2017

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
#include "main/duq.h"
#include "classes/species.h"

// Setup options for module
void PartialsModule::setupOptions()
{
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	frequency_ = 5;
	options_.add("Bragg", bool(false), "Enable calculation of Bragg scattering", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("BraggQDepBroadening", 0.0063, "FWHM of Gaussian for Q-dependent Bragg broadening function", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("BraggQIndepBroadening", 0.0, "FWHM of Gaussian for Q-independent Bragg broadening function", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("NormaliseToAvSq", bool(false), "Normalise calculated F(Q) to < b >**2", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("NormaliseToSqAv", bool(false), "Normalise calculated F(Q) to < b**2 >", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QDelta", 0.05, "Step size in Q for S(Q) calculation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QDepBroadening", 0.0, "FWHM of Gaussian for Q-dependent instrument broadening function when calculating S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QIndepBroadening", 0.0, "FWHM of Gaussian for Q-independent instrument broadening function when calculating S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QMax", -1.0, "Maximum Q for calculated S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("QMin", 0.0, "Minimum Q for calculated S(Q)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Save", bool(false), "Whether to save partials to disk after calculation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Smoothing", 0, "Specifies the degree of smoothing 'n' to apply to calculated RDFs, where 2n+1 controls the length in the applied Spline smooth", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("StructureFactor", bool(false), "Determines whether S(Q) are to be calculated from F.T. of the g(r)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Test", bool(false), "Test calculated total and partials against supplied reference data", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TestThreshold", 0.1, "Test threshold (RMSE) above which test fails", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("Weights", "None", "Weighting scheme to use for calculated partials (None,Neutron)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int PartialsModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	if (DUQSys::sameString(parser.argc(0), "Isotopologue"))
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

		if (!targetCfg->usedSpecies().contains(sp))
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
		XYData& data = GenericListHelper<XYData>::realise(targetList, CharString("TestReference%s", parser.argc(2)), uniqueName(), GenericItem::NoOutputFlag);

		// Fourth and fifth arguments are x and y columns respectively (defaulting to 0,1 if not given)
		int xcol = parser.hasArg(3) ? parser.argi(3)-1 : 0;
		int ycol = parser.hasArg(4) ? parser.argi(4)-1 : 1;

		LineParser fileParser(&duq->worldPool());
		if ((!fileParser.openInput(parser.argc(1))) || (!data.load(fileParser, xcol, ycol))) return false;
	}
	else return -1;

	return true;
}

