/*
	*** Module Keyword - Complex
	*** src/modules/modulekeyword_complex.cpp
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

#include "modules/modulekeyword_complex.h"

// Constructors
ComplexModuleKeyword::ComplexModuleKeyword(int minArgs, int maxArgs) : ModuleKeywordBase(ModuleKeywordBase::ComplexData), ModuleKeywordData<int>(0)
{
	minArguments_ = minArgs;
	maxArguments_ = maxArgs;
}

// Destructor
ComplexModuleKeyword::~ComplexModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void ComplexModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ComplexModuleKeyword::minArguments()
{
	return minArguments_;
}

// Return maximum number of arguments accepted
int ComplexModuleKeyword::maxArguments()
{
	return maxArguments_;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool ComplexModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	return false;
}
