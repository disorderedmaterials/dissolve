/*
	*** Module Keyword - CharString
	*** src/module/keywordtypes/charstring.cpp
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

#include "module/keywordtypes/charstring.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
CharStringModuleKeyword::CharStringModuleKeyword(CharString value) : ModuleKeywordData<CharString>(ModuleKeywordBase::CharStringData, value)
{
}

// Destructor
CharStringModuleKeyword::~CharStringModuleKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int CharStringModuleKeyword::minArguments()
{
	return 1;
}

// Return minimum number of arguments accepted
int CharStringModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool CharStringModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argc(startArg))) return false;

		return true;
	}

	return false;
}

// Write keyword data to specified LineParser
bool CharStringModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_.get());
}

/*
 * Conversion
 */

// Return value (as bool)
bool CharStringModuleKeyword::asBool()
{
	return DissolveSys::atob(data_);
}

// Return value (as int)
int CharStringModuleKeyword::asInt()
{
	return atoi(data_);
}

// Return value (as double)
double CharStringModuleKeyword::asDouble()
{
	return atof(data_);
}

// Return value (as string)
const char* CharStringModuleKeyword::asString()
{
	return data_;
}
