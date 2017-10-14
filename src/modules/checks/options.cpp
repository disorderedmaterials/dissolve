/*
	*** Checks Module - Options
	*** src/modules/checks/options.cpp
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

#include "modules/checks/checks.h"
#include "base/lineparser.h"

// Setup options for module
void ChecksModule::setupOptions()
{
	// Boolean options must be set as 'bool(false)' or 'bool(true)' rather than just 'false' or 'true' so that the correct overloaded add() function is called
	options_.add("AngleThreshold", 0.05, "Threshold at which angle checks will fail (degrees)")->setValidationMin(1.0-5);
	options_.add("DistanceThreshold", 0.001, "Threshold at which distance checks will fail (Angstroms)")->setValidationMin(1.0-5);
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ChecksModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	printf("HERE WE ARE %s\n", parser.argc(0));
	if (DUQSys::sameString(parser.argc(0), "Angle"))
	{
		if (parser.nArgs() != 5)
		{
			Messenger::error("Wrong number of arguments given to 'Angle' keyword.\n");
			return 0;
		}
		Geometry* g = angles_.add();
		g->set(parser.argd(4), parser.argi(1)-1, parser.argi(2)-1, parser.argi(3)-1);

		return 1;
	}
	else if (DUQSys::sameString(parser.argc(0), "Distance"))
	{
		if (parser.nArgs() != 4)
		{
			Messenger::error("Wrong number of arguments given to 'Distance' keyword.\n");
			return 0;
		}
		Geometry* g = distances_.add();
		g->set(parser.argd(3), parser.argi(1)-1, parser.argi(2)-1);

		return 1;
	}

	return -1;
}
