/*
	*** Module Keyword - Function
	*** src/modules/modulekeyword_function.cpp
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

#include "modules/modulekeyword_function.h"
#include "base/lineparser.h"
#include "templates/genericlist.h"

// Constructor
FunctionModuleKeyword::FunctionModuleKeyword(Function value) : ModuleKeywordBase(ModuleKeywordBase::FunctionData), ModuleKeywordData<Function>(value)
{
}

// Destructor
FunctionModuleKeyword::~FunctionModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void FunctionModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<Function>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int FunctionModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int FunctionModuleKeyword::maxArguments()
{
	return MAXFUNCTIONPARAMS;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool FunctionModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	return data_.set(parser, startArg);
}
