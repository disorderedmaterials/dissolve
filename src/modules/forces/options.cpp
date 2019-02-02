/*
	*** Forces Module - Options
	*** src/modules/forces/options.cpp
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

#include "modules/forces/forces.h"
#include "modules/import/import.h"
#include "module/keywordtypes.h"
#include "main/dissolve.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Set up keywords for Module
void ForcesModule::setUpKeywords()
{
	// Test
	ModuleKeywordGroup* group = addKeywordGroup("Test");
	group->add(new BoolModuleKeyword(false), "Test", "Test parallel force routines against simplified, serial ones");
	group->add(new BoolModuleKeyword(false), "TestAnalytic", "Compare parallel force routines against exact (analytic) force rather than tabulated values");
	group->add(new BoolModuleKeyword(true), "TestInter", "Include interatomic forces in test");
	group->add(new BoolModuleKeyword(true), "TestIntra", "Include intramolecular forces in test");
	group->add(new FileAndFormatModuleKeyword(referenceForces_), "TestReference", "Reference forces for test");
	group->add(new DoubleModuleKeyword(0.1), "TestThreshold", "Threshold of force (%%) at which test comparison will fail");

	// Export
	group = addKeywordGroup("Export");
	group->add(new BoolModuleKeyword(false), "Save", "Save forces for the Configuration to the file '<name>.forces.txt'");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ForcesModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
