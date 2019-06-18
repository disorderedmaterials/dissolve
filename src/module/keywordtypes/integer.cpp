/*
	*** Module Keyword - Integer
	*** src/module/keywordtypes/integer.cpp
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

#include "module/keywordtypes/integer.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructors
IntegerModuleKeyword::IntegerModuleKeyword(int value) : ModuleKeywordData<int>(ModuleKeywordBase::IntegerData, value)
{
	minimumLimit_ = false;
	maximumLimit_ = false;
}

IntegerModuleKeyword::IntegerModuleKeyword(int value, int minValue) : ModuleKeywordData<int>(ModuleKeywordBase::IntegerData, value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = false;
}

IntegerModuleKeyword::IntegerModuleKeyword(int value, int minValue, int maxValue) : ModuleKeywordData<int>(ModuleKeywordBase::IntegerData, value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = true;
	max_ = maxValue;
}

// Destructor
IntegerModuleKeyword::~IntegerModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool IntegerModuleKeyword::isSet()
{
	return set_;
}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool IntegerModuleKeyword::hasValidationMin()
{
	return minimumLimit_;
}

// Return validation minimum limit
int IntegerModuleKeyword::validationMin()
{
	return min_;
}

// Return whether a maximum validation limit has been set
bool IntegerModuleKeyword::hasValidationMax()
{
	return maximumLimit_;
}

// Return validation maximum limit
int IntegerModuleKeyword::validationMax()
{
	return max_;
}

// Validate supplied value
bool IntegerModuleKeyword::isValid(int value)
{
	// Check minimum limit
	if (minimumLimit_)
	{
		if (value < min_) return false;
	}

	// Check maximum limit
	if (maximumLimit_)
	{
		if (value > max_) return false;
	}

	return true;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IntegerModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int IntegerModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool IntegerModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argi(startArg)))
		{
			if (minimumLimit_ && maximumLimit_) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n <= %i.\n", data_, min_, max_);
			else if (minimumLimit_) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n.\n", data_, min_);
			else Messenger::error("Value %i is out of range for keyword. Valid range is n <= %i.\n", data_, max_);

			return false;
		}

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool IntegerModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %i\n", prefix, keyword(), data_);
}

/*
 * Conversion
 */

// Return value (as bool)
bool IntegerModuleKeyword::asBool()
{
	return data_;
}

// Return value (as int)
int IntegerModuleKeyword::asInt()
{
	return data_;
}

// Return value (as double)
double IntegerModuleKeyword::asDouble()
{
	return data_*1.0;
}

// Return value (as string)
const char* IntegerModuleKeyword::asString()
{
	return DissolveSys::itoa(data_);
}
