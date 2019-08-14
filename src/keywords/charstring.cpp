/*
	*** Keyword - CharString
	*** src/keywords/charstring.cpp
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

#include "keywords/charstring.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
CharStringKeyword::CharStringKeyword(CharString value) : KeywordData<CharString>(KeywordBase::CharStringData, value)
{
}

// Destructor
CharStringKeyword::~CharStringKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int CharStringKeyword::minArguments()
{
	return 1;
}

// Return minimum number of arguments accepted
int CharStringKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool CharStringKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argc(startArg))) return false;

		return true;
	}

	return false;
}

// Write keyword data to specified LineParser
bool CharStringKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_.get());
}

/*
 * Conversion
 */

// Return value (as bool)
bool CharStringKeyword::asBool()
{
	return DissolveSys::atob(data_);
}

// Return value (as int)
int CharStringKeyword::asInt()
{
	return atoi(data_);
}

// Return value (as double)
double CharStringKeyword::asDouble()
{
	return atof(data_);
}

// Return value (as string)
const char* CharStringKeyword::asString()
{
	return data_;
}
