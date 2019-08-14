/*
	*** Keyword - Bool
	*** src/keywords/bool.cpp
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

#include "keywords/bool.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
BoolKeyword::BoolKeyword(bool value) : KeywordData<bool>(KeywordBase::BoolData, value)
{
}

// Destructor
BoolKeyword::~BoolKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BoolKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int BoolKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool BoolKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		setData(parser.argb(startArg));

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool BoolKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %s\n", prefix, keyword(), DissolveSys::btoa(data_));
}

/*
 * Conversion
 */

// Return value (as bool)
bool BoolKeyword::asBool()
{
	return data_;
}

// Return value (as int)
int BoolKeyword::asInt()
{
	return data_ ? 1 : 0;
}

// Return value (as double)
double BoolKeyword::asDouble()
{
	return data_ ? 1.0 : 0.0;
}

// Return value (as string)
const char* BoolKeyword::asString()
{
	return DissolveSys::btoa(data_);
}
