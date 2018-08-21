/*
	*** Module Keyword - CharString
	*** src/module/keywordtypes/charstring.cpp
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

#include "module/keywordtypes/charstring.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
CharStringModuleKeyword::CharStringModuleKeyword(CharString value) : ModuleKeywordBase(ModuleKeywordBase::CharStringData), ModuleKeywordData<CharString>(value)
{
}

CharStringModuleKeyword::CharStringModuleKeyword(CharString value, int nOptions, const char** options) : ModuleKeywordBase(ModuleKeywordBase::CharStringData), ModuleKeywordData<CharString>(value)
{
	// Set our array of valid values
	for (int n=0; n<nOptions; ++n) validValues_.add(options[n]);
}

// Destructor
CharStringModuleKeyword::~CharStringModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool CharStringModuleKeyword::isSet()
{
	return set_;
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
bool CharStringModuleKeyword::read(LineParser& parser, int startArg, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		if (!setData(parser.argc(startArg)))
		{
			CharString validValueString;
			for (int n=0; n<validValues_.nItems(); ++n) validValueString += CharString(n == 0 ? "%s" : ", %s", validValues_[n].get());
			Messenger::error("Value '%s' is not valid for this keyword.\nValid values are:  %s", data_.get(), validValueString.get());

			return false;
		}

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
 * Validation
 */

// Return whether a validation list has been set
bool CharStringModuleKeyword::hasValidationList()
{
	return (validValues_.nItems() > 0);
}

// Return validation list
const Array<CharString>& CharStringModuleKeyword::validationList()
{
	return validValues_;
}

// Validate supplied value
bool CharStringModuleKeyword::isValid(CharString value)
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
