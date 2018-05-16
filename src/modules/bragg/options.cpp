/*
	*** Bragg Module - Options
	*** src/modules/bragg/options.cpp
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

#include "modules/bragg/bragg.h"
#include "module/keywordtypes.h"
#include "main/duq.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/enumhelpers.h"
#include "templates/genericlisthelper.h"

// Set up keywords for Module
void BraggModule::setUpKeywords()
{
	frequency_ = 5;
	keywords_.add(new ComplexModuleKeyword(2,2), "ConfigurationWeight", "Sets the relative weight of the specified Configuration in construction of the partials", "<Configuration Name> <weight>");
	keywords_.add(new DoubleModuleKeyword(0.001), "QResolution", "Binwidth in Q to use when calculating Bragg peaks");
	keywords_.add(new BoolModuleKeyword(false), "Save", "Whether to save partials to disk after calculation", "<True|False>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int BraggModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
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
	else return -1;

	return true;
}

