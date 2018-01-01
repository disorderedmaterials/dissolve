/*
	*** Forces Module - Options
	*** src/modules/forces/options.cpp
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

#include "modules/forces/forces.h"
#include "modules/import/import.h"
#include "module/keywordtypes.h"
#include "main/duq.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Set up keywords for Module
void ForcesModule::setUpKeywords()
{
	keywords_.add(new BoolModuleKeyword(false), "Save", "Save forces for the Configuration to the file '<name>.forces.txt'");
	keywords_.add(new BoolModuleKeyword(false), "Test", "Test parallel force routines against simplified, serial ones");
	keywords_.add(new BoolModuleKeyword(false), "TestAnalytic", "Compare parallel force routines against exact (analytic) force rather than tabulated values");
	keywords_.add(new BoolModuleKeyword(true), "TestInter", "Include interatomic forces in test");
	keywords_.add(new BoolModuleKeyword(true), "TestIntra", "Include intramolecular forces in test");
	keywords_.add(new ComplexModuleKeyword(1,2), "TestReference", "Filename containing reference forces for test");
	keywords_.add(new DoubleModuleKeyword(0.1), "TestThreshold", "Threshold of force (%%) at which test comparison will fail");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ForcesModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString(parser.argc(0), "TestReference"))
	{
		Messenger::print("Reading test reference forces.\n");

		// Realise some arrays to store the forces in
		Array<double>& fx = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFX", uniqueName());
		Array<double>& fy = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFY", uniqueName());
		Array<double>& fz = GenericListHelper< Array<double> >::realise(targetList, "ReferenceFZ", uniqueName());

		// Second argument is the format, third (if present) is the target file
		if (parser.hasArg(2))
		{
			LineParser fileParser(&duq->worldPool());
			if (!fileParser.openInput(parser.argc(2))) return 0;

			return ImportModule::readForces(parser.argc(1), fileParser, fx, fy, fz);
		}
		else return ImportModule::readForces(parser.argc(1), parser, fx, fy, fz);
	}

	return -1;
}
