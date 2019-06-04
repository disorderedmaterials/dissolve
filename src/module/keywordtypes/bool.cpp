/*
	*** Module Keyword - Bool
	*** src/module/keywordtypes/bool.cpp
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

#include "module/keywordtypes/bool.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
BoolModuleKeyword::BoolModuleKeyword(bool value) : ModuleKeywordBase(ModuleKeywordBase::BoolData), ModuleKeywordData<bool>(value)
{
}

// Destructor
BoolModuleKeyword::~BoolModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool BoolModuleKeyword::isSet()
{
	return set_;
}

// Validate supplied value
bool BoolModuleKeyword::isValid(bool value)
{
	return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BoolModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int BoolModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool BoolModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		setData(parser.argb(startArg));

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool BoolModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %s\n", prefix, keyword(), DissolveSys::btoa(data_));
}

/*
 * Conversion
 */

// Return value (as bool)
bool BoolModuleKeyword::asBool()
{
	return data_;
}

// Return value (as int)
int BoolModuleKeyword::asInt()
{
	return data_;
}

// Return value (as double)
double BoolModuleKeyword::asDouble()
{
	return data_*1.0;
}

// Return value (as string)
const char* BoolModuleKeyword::asString()
{
	return DissolveSys::btoa(data_);
}
