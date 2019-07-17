/*
	*** Module Keyword - Window Function
	*** src/modules/keywordtypes/windowfunction.cpp
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

#include "module/keywordtypes/windowfunction.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
WindowFunctionModuleKeyword::WindowFunctionModuleKeyword(WindowFunction value) : ModuleKeywordData<WindowFunction>(ModuleKeywordBase::WindowFunctionData, value)
{
}

// Destructor
WindowFunctionModuleKeyword::~WindowFunctionModuleKeyword()
{
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

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool WindowFunctionModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	bool result = data_.set(parser, startArg);
	if (result) set_ = true;

	return result;
}

// Write keyword data to specified LineParser
bool WindowFunctionModuleKeyword::write(LineParser& parser, const char* prefix)
{
	CharString params;
	for (int n=0; n<WindowFunction::nFunctionParameters(data_.function()); ++n) params.strcatf("  %f", data_.parameter(n));
	return parser.writeLineF("%s%s  '%s'%s\n", prefix, keyword(), WindowFunction::functionType(data_.function()), params.get());
}
