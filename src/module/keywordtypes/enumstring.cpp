/*
	*** Module Keyword - Enum String
	*** src/module/keywordtypes/enumstring.cpp
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

#include "module/keywordtypes/enumstring.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

EnumStringModuleKeyword::EnumStringModuleKeyword(int value, int nOptions, const char** options) : ModuleKeywordBase(ModuleKeywordBase::EnumStringData), ModuleKeywordData<CharString>(options[value])
{
	// Set our array of valid values
	for (int n=0; n<nOptions; ++n) validValues_.add(options[n]);
}

// Destructor
EnumStringModuleKeyword::~EnumStringModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool EnumStringModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int EnumStringModuleKeyword::minArguments()
{
	return 1;
}

// Return minimum number of arguments accepted
int EnumStringModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool EnumStringModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argc(startArg)))
		{
			CharString validValueString;
			for (int n=0; n<validValues_.nItems(); ++n) validValueString += CharString(n == 0 ? "%s" : ", %s", validValues_[n].get());
			Messenger::error("Value '%s' is not valid for the '%s' keyword.\nValid values are:  %s", parser.argc(startArg), keyword(), validValueString.get());

			return false;
		}

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool EnumStringModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_.get());
}

/*
 * Validation
 */

// Return whether a validation list has been set
bool EnumStringModuleKeyword::hasValidationList()
{
	return (validValues_.nItems() > 0);
}

// Return validation list
const Array<CharString>& EnumStringModuleKeyword::validationList()
{
	return validValues_;
}

// Validate supplied value
bool EnumStringModuleKeyword::isValid(CharString value)
{
	// If there is no validation list, we always accept the value
	if (validValues_.nItems() == 0) return true;

	for (int n=0; n<validValues_.nItems(); ++n) if (DissolveSys::sameString(value, validValues_[n])) return true;

	return false;
}

/*
 * Conversion
 */

// Return value (as bool)
bool EnumStringModuleKeyword::asBool()
{
	return DissolveSys::atob(data_);
}

// Return value (as int)
int EnumStringModuleKeyword::asInt()
{
	return atoi(data_);
}

// Return value (as double)
double EnumStringModuleKeyword::asDouble()
{
	return atof(data_);
}

// Return value (as string)
const char* EnumStringModuleKeyword::asString()
{
	return data_;
}
