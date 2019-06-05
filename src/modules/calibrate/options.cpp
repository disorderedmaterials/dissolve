/*
	*** Calibration Module - Options
	*** src/modules/calibration/options.cpp
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

#include "modules/calibrate/calibrate.h"
#include "module/keywordtypes.h"
#include "module/list.h"
#include "genericitems/listhelper.h"

// Set up keywords for Module
void CalibrationModule::setUpKeywords()
{
	// Calculation
	ModuleKeywordGroup* group = addKeywordGroup("Calculation");
	group->add(new BoolModuleKeyword(true), "OnlyWhenEnergyStable", "Only perform calibrations when all related Configuration energies are stable");

	// RDF Calibration
	group = addKeywordGroup("RDF Calibration");
	group->add(new ModuleReferenceListModuleKeyword(intraBroadeningModules_, "RDF"), "AdjustIntraBroadening", "Add specified RDF module as a target for IntraBroadening adjustment", "<RDFModule>");

	// NeutronSQ Calibration
	group = addKeywordGroup("NeutronSQ Calibration");
	group->add(new ModuleReferenceListModuleKeyword(intraBroadeningNeutronGRReferences_, "NeutronSQ"), "IntraBroadeningNeutronGRReference", "Add G(r) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment", "<NeutronSQModule>");
	group->add(new ModuleReferenceListModuleKeyword(intraBroadeningNeutronSQReferences_, "NeutronSQ"), "IntraBroadeningNeutronSQReference", "Add S(Q) data in the specified NeutronSQ module as a reference for IntraBroadening adjustment", "<NeutronSQModule>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int CalibrationModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
