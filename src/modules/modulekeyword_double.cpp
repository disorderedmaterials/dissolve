/*
	*** Module Keyword - Double
	*** src/modules/modulekeyword_double.cpp
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

#include "modules/modulekeyword_double.h"
#include "base/lineparser.h"
#include "templates/genericlist.h"

// Constructors
DoubleModuleKeyword::DoubleModuleKeyword(double value) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
}

DoubleModuleKeyword::DoubleModuleKeyword(double value, double minValue) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
	setValidationMin(minValue);
}

DoubleModuleKeyword::DoubleModuleKeyword(double value, double minValue, double maxValue) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
	setValidationRange(minValue, maxValue);
}

// Destructor
DoubleModuleKeyword::~DoubleModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void DoubleModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<double>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int DoubleModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int DoubleModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool DoubleModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	if (parser.hasArg(startArg))
	{
		data_ = parser.argd(startArg);
		return true;
	}
	return false;
}


/*
 * Conversion
 */

// Return value (as bool)
bool DoubleModuleKeyword::asBool()
{
	return data_;
}

// Return value (as int)
int DoubleModuleKeyword::asInt()
{
	return data_;
}

// Return value (as double)
double DoubleModuleKeyword::asDouble()
{
	return data_;
}

// Return value (as string)
const char* DoubleModuleKeyword::asString()
{
	return DUQSys::ftoa(data_);
}
