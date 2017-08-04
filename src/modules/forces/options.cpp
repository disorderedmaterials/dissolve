/*
	*** Forces Module - Options
	*** src/modules/forces/options.cpp
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

#include "modules/forces/forces.h"
#include "modules/import/import.h"
#include "main/duq.h"

// Setup options for module
void ForcesModule::setupOptions()
{
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("Save", bool(false), "Save forces for the Configuration to the file '<name>.forces.txt'");
	options_.add("Test", bool(false), "Test parallel force routines against simplified, serial ones");
	options_.add("TestAnalytic", bool(false), "Compare parallel force routines against exact (analytic) force rather than tabulated values");
	options_.add("TestInter", bool(true), "Include interatomic forces in test");
	options_.add("TestIntra", bool(true), "Include intramolecular forces in test");
	options_.add("TestThreshold", 0.1, "Threshold of force (%%) at which test comparison will fail");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ForcesModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
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
