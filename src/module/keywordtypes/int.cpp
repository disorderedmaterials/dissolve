/*
	*** Module Keyword - Integer
	*** src/modules/modulekeyword_int.cpp
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

#include "module/keywordtypes/int.h"
#include "base/lineparser.h"
#include "templates/genericlist.h"

// Constructors
IntegerModuleKeyword::IntegerModuleKeyword(int value) : ModuleKeywordBase(ModuleKeywordBase::IntegerData), ModuleKeywordData<int>(value)
{
}

IntegerModuleKeyword::IntegerModuleKeyword(int value, int minValue) : ModuleKeywordBase(ModuleKeywordBase::IntegerData), ModuleKeywordData<int>(value)
{
	setValidationMin(minValue);
}

IntegerModuleKeyword::IntegerModuleKeyword(int value, int minValue, int maxValue) : ModuleKeywordBase(ModuleKeywordBase::IntegerData), ModuleKeywordData<int>(value)
{
	setValidationRange(minValue, maxValue);
}

// Destructor
IntegerModuleKeyword::~IntegerModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void IntegerModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<int>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
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

// Parse arguments from supplied LineParser, starting at argument offset specified
bool IntegerModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	if (parser.hasArg(startArg))
	{
		// Grab data value
		data_ = parser.argi(startArg);

		// Check it against any defined validation limits
		if (!isValid(data_))
		{
			if (listLimit_)
			{
				CharString validValues;
				for (int n=0; n<allowedValues_.nItems(); ++n) validValues += CharString(n == 0 ? "%i" : ", %i", allowedValues_[n]);
				Messenger::error("Value '%i' is not valid for this keyword.\nValid values are:  %s", data_, validValues.get());
			}
			else if (minimumLimit_ && maximumLimit_) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n <= %i.\n", data_, min_, max_);
			else if (minimumLimit_) Messenger::error("Value %i is out of range for keyword. Valid range is %i <= n.\n", data_, min_);
			else Messenger::error("Value %i is out of range for keyword. Valid range is n <= %i.\n", data_, max_);

			return false;
		}

		return true;
	}
	return false;
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
	return DUQSys::itoa(data_);
}
