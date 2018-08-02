/*
	*** SQ Module - Options
	*** src/modules/sq/options.cpp
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

#include "modules/sq/sq.h"
#include "module/keywordtypes.h"
#include "main/dissolve.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Set up keywords for Module
void SQModule::setUpKeywords()
{
	frequency_ = 5;
	keywords_.add(new ComplexModuleKeyword(2,2), "ConfigurationWeight", "Sets the relative weight of the specified Configuration in construction of the structure factors", "<Configuration Name> <weight>");
	keywords_.add(new DoubleModuleKeyword(0.05, 1.0e-5), "QDelta", "Step size in Q for S(Q) calculation");
	keywords_.add(new BroadeningFunctionModuleKeyword(BroadeningFunction()), "QBroadening", "Instrument broadening function to apply when calculating S(Q)");
	keywords_.add(new DoubleModuleKeyword(-1.0, -1.0), "QMax", "Maximum Q for calculated S(Q)");
	keywords_.add(new DoubleModuleKeyword(0.01, 0.0), "QMin", "Minimum Q for calculated S(Q)");
	keywords_.add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test calculated total and partials against supplied reference data", "<True|False>");
	keywords_.add(new XYDataStoreModuleKeyword(testData_), "TestReference", "Specify test reference data", "<filename> <target> [xcol] [ycol]");
	keywords_.add(new DoubleModuleKeyword(0.1, 1.0e-5), "TestThreshold", "Test threshold (%%error) above which test fails", "<threshold[0.1]>");
	keywords_.add(new WindowFunctionModuleKeyword(WindowFunction(WindowFunction::Lorch0Window)), "WindowFunction", "Window function to apply in Fourier-transform of g(r) to S(Q)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int SQModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	if (DissolveSys::sameString(parser.argc(0), "ConfigurationWeight"))
	{
		// Sets the weight of a specified Configuration in construction of the partials
		// Find target Configuration
		Configuration* targetCfg = dissolve->findConfiguration(parser.argc(1));
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
	else return -1;

	return true;
}
