/*
	*** Test Module - Options
	*** src/modules/test/options.cpp
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

#include "modules/test/test.h"
#include "module/list.h"
#include "module/keywordtypes.h"
#include "base/lineparser.h"

// Set up keywords for Module
void TestModule::setUpKeywords()
{
// 	keywords_.add(new CharStringModuleKeyword("<Undefined>"), "Partials", "Partials Module from which to get simulated partial data");
	keywords_.add(new BoolModuleKeyword(true), "OnlyWhenStable", "Assesses the energy of the Configurations contributing to the Partials, refining the potential only when all energies are stable");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int TestModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, DUQ* duq, GenericList& targetList, const char* prefix)
{
	if (DUQSys::sameString("__", parser.argc(0)))
	{
	}
	else return -1;

	return true;
}
