/*
	*** Calibration Module - Options
	*** src/modules/calibration/options.cpp
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

#include "modules/calibration/calibration.h"
#include "module/keywordtypes.h"
#include "module/list.h"
#include "templates/genericlisthelper.h"

// IntraBroadening Fitting Targets
const char* IntraBroadeningFitTargetKeywords[] = { "S(Q)", "g(r)", "Both" };

// Convert string to functional form
CalibrationModule::IntraBroadeningFitTarget CalibrationModule::intraBroadeningFitTarget(const char* s)
{
	for (int n=0; n<CalibrationModule::nIntraBroadeningFitTargets; ++n) if (DUQSys::sameString(s, IntraBroadeningFitTargetKeywords[n])) return (CalibrationModule::IntraBroadeningFitTarget) n;
	return CalibrationModule::nIntraBroadeningFitTargets;
}

// Return fit target text
const char* CalibrationModule::intraBroadeningFitTarget(CalibrationModule::IntraBroadeningFitTarget ft)
{
	return IntraBroadeningFitTargetKeywords[ft];
}

// Set up keywords for Module
void CalibrationModule::setUpKeywords()
{
	keywords_.add(new ComplexModuleKeyword(1,1), "AdjustIntraBroadening", "Add specified RDF module as a target for IntraBroadening adjustment", "<RDFModule>");
	keywords_.add(new ComplexModuleKeyword(1,2), "IntraBroadeningNeutronReference", "Add specified NeutronSQ module as a reference for IntraBroadening adjustment", "<NeutronSQModule> [target=S(Q)]");
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenStable", "Only perform calibrations when all related Configuration energies are stable");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalibrationModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString(parser.argc(0), "AdjustIntraBroadening"))
	{
		// Find specified RDFModule
		Module* module = ModuleList::findInstanceByUniqueName(parser.argc(1));
		if (!module)
		{
			Messenger::error("Error adding RDFModule fitting target - no Module named '%s' exists.\n", parser.argc(1));
			return false;
		}
		if (!DUQSys::sameString(module->name(), "RDF"))
		{
			Messenger::error("Error adding RDFModule fitting target - Module '%s' is not an RDF Module (%s).\n", parser.argc(1), module->name());
			return false;
		}

		intraBroadeningModules_.add(module);
	}
	else 	if (DUQSys::sameString(parser.argc(0), "IntraBroadeningNeutronReference"))
	{
		// Find specified RDFModule
		Module* module = ModuleList::findInstanceByUniqueName(parser.argc(1));
		if (!module)
		{
			Messenger::error("Error adding NeutronSQ reference target - no Module named '%s' exists.\n", parser.argc(1));
			return false;
		}
		if (!DUQSys::sameString(module->name(), "NeutronSQ"))
		{
			Messenger::error("Error adding NeutronSQ reference target - Module '%s' is not a NeutronSQ Module (%s).\n", parser.argc(1), module->name());
			return false;
		}

		// Was a specific target supplied?
		CalibrationModule::IntraBroadeningFitTarget target = CalibrationModule::IntraBroadeningTargetSQ;
		if (parser.hasArg(2))
		{
			target = CalibrationModule::intraBroadeningFitTarget(parser.argc(2));
			if (target == CalibrationModule::nIntraBroadeningFitTargets) return false;
		}

		intraBroadeningReferences_.add(module, target);
	}
	else return -1;

	return true;
}
