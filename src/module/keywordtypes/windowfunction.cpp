/*
	*** Module Keyword - Window Function
	*** src/modules/keywordtypes/windowfunction.cpp
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

#include "module/keywordtypes/windowfunction.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
WindowFunctionModuleKeyword::WindowFunctionModuleKeyword(WindowFunction value) : ModuleKeywordBase(ModuleKeywordBase::WindowFunctionData), ModuleKeywordData<WindowFunction>(value)
{
}

// Destructor
WindowFunctionModuleKeyword::~WindowFunctionModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void WindowFunctionModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<WindowFunction>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

// Return whether the current data value has ever been set
bool WindowFunctionModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int WindowFunctionModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int WindowFunctionModuleKeyword::maxArguments()
{
	return MAXWINDOWFUNCTIONPARAMS;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool WindowFunctionModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	bool result = data_.set(parser, startArg);
	if (result) set_ = true;

	return result;
}

/*
 * Validation
 */

// Validate supplied value
bool WindowFunctionModuleKeyword::isValid(WindowFunction value)
{
	return true;
}
