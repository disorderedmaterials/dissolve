/*
	*** Module Keyword - Complex
	*** src/module/keywordtypes/complex.cpp
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

#include "module/keywordtypes/complex.h"

// Constructors
ComplexModuleKeyword::ComplexModuleKeyword(int minArgs, int maxArgs) : ModuleKeywordData<int>(ModuleKeywordBase::ComplexData, 0)
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

// Return whether the current data value has ever been set
bool ComplexModuleKeyword::isSet()
{
	return set_;
}

// Validate supplied value
bool ComplexModuleKeyword::isValid(int value)
{
	return false;
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

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool ComplexModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	return false;
}

// Write keyword data to specified LineParser
bool ComplexModuleKeyword::write(LineParser& parser, const char* prefix)
{
	Messenger::print("Tried to write a ComplexModuleKeyword - remove this horrible class!\n");
	return true;
}
