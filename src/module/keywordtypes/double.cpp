/*
	*** Module Keyword - Double
	*** src/module/keywordtypes/double.cpp
	Copyright T. Youngs 2012-2018

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

#include "module/keywordtypes/double.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructors
DoubleModuleKeyword::DoubleModuleKeyword(double value) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
	minimumLimit_ = false;
	maximumLimit_ = false;
}

DoubleModuleKeyword::DoubleModuleKeyword(double value, double minValue) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = false;
}

DoubleModuleKeyword::DoubleModuleKeyword(double value, double minValue, double maxValue) : ModuleKeywordBase(ModuleKeywordBase::DoubleData), ModuleKeywordData<double>(value)
{
	minimumLimit_ = true;
	min_ = minValue;
	maximumLimit_ = true;
	max_ = maxValue;
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

// Return whether the current data value has ever been set
bool DoubleModuleKeyword::isSet()
{
	return set_;
}

/*
 * Data Validation
 */

// Return whether a minimum validation limit has been set
bool DoubleModuleKeyword::hasValidationMin()
{
	return minimumLimit_;
}

// Return validation minimum limit
double DoubleModuleKeyword::validationMin()
{
	return min_;
}

// Return whether a maximum validation limit has been set
bool DoubleModuleKeyword::hasValidationMax()
{
	return maximumLimit_;
}

// Return validation maximum limit
double DoubleModuleKeyword::validationMax()
{
	return max_;
}

// Validate supplied value
bool DoubleModuleKeyword::isValid(double value)
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
int DoubleModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int DoubleModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool DoubleModuleKeyword::parseArguments(LineParser& parser, int startArg, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argd(startArg)))
		{
			if (minimumLimit_ && maximumLimit_) Messenger::error("Value %f is out of range for keyword. Valid range is %f <= n <= %f.\n", data_, min_, max_);
			else if (minimumLimit_) Messenger::error("Value %f is out of range for keyword. Valid range is %f <= n.\n", data_, min_);
			else Messenger::error("Value %f is out of range for keyword. Valid range is n <= %f.\n", data_, max_);

			return false;
		}

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool DoubleModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  %12.5e\n", prefix, keyword(), data_);
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
	return DissolveSys::ftoa(data_);
}
