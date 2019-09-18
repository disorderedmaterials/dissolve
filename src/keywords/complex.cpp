/*
	*** Keyword - Complex
	*** src/keywords/complex.cpp
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

#include "keywords/complex.h"

// Constructors
ComplexKeyword::ComplexKeyword(int minArgs, int maxArgs) : KeywordData<int>(KeywordBase::ComplexData, 0)
{
	minArguments_ = minArgs;
	maxArguments_ = maxArgs;
}

// Destructor
ComplexKeyword::~ComplexKeyword()
{
}

/*
 * Data
 */

// Validate supplied value
bool ComplexKeyword::isValid(int value)
{
	return false;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ComplexKeyword::minArguments() const
{
	return minArguments_;
}

// Return maximum number of arguments accepted
int ComplexKeyword::maxArguments() const
{
	return maxArguments_;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool ComplexKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	return false;
}

// Write keyword data to specified LineParser
bool ComplexKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	Messenger::print("Tried to write a ComplexKeyword - remove this horrible class!\n");
	return true;
}
