/*
	*** Checks Module - Options
	*** src/modules/checks/options.cpp
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

#include "modules/checks/checks.h"
#include "module/keywordtypes.h"
#include "base/lineparser.h"

// Set up keywords for Module
void ChecksModule::setUpKeywords()
{
	keywords_.add(new ComplexModuleKeyword(4,4), "Angle", "Define an angle between Atoms to be checked", "<i> <j> <k> <referenceAngle>");
	keywords_.add(new DoubleModuleKeyword(0.05, 1.0e-5), "AngleThreshold", "Threshold at which angle checks will fail", "<threshold[0.05]>");
	keywords_.add(new ComplexModuleKeyword(3,3), "Distance", "Define a distance between Atoms to be checked", "<i> <j> <referenceDistance>");
	keywords_.add(new DoubleModuleKeyword(0.001, 1.0e-5), "DistanceThreshold", "Threshold at which distance checks will fail (Angstroms)", "<threshold[0.001]>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ChecksModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	if (DissolveSys::sameString(parser.argc(0), "Angle"))
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
	else if (DissolveSys::sameString(parser.argc(0), "Distance"))
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
