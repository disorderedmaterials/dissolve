/*
	*** Module Keyword - EnumOptions
	*** src/module/keywordtypes/enumoptions.cpp
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

#include "keywords/enumoptions.h"
#include "base/enumoptions.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
EnumOptionsModuleKeyword::EnumOptionsModuleKeyword(EnumOptionsBase options) : ModuleKeywordData<EnumOptionsBase>(ModuleKeywordBase::EnumOptionsData, options)
{
	// Set our array of valid values
	for (int n=0; n<data_.nOptions(); ++n) validKeywords_.add(data_.keywordByIndex(n));
}

// Destructor
EnumOptionsModuleKeyword::~EnumOptionsModuleKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int EnumOptionsModuleKeyword::minArguments()
{
	return 1;
}

// Return minimum number of arguments accepted
int EnumOptionsModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool EnumOptionsModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (parser.hasArg(startArg))
	{
		// Check validity of the supplied keyword...
		if (!data_.isValid(parser.argc(startArg))) return data_.errorAndPrintValid(parser.argc(startArg));

		// Keyword recognised...
		EnumOptionsBase newOptions(data_);
		newOptions.setCurrentOption(parser.argc(startArg));
		if (!setData(newOptions)) return Messenger::error("An odd thing happened....\n");

		return true;
	}
	return false;
}

// Write keyword data to specified LineParser
bool EnumOptionsModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_.currentOptionKeyword());
}

/*
 * Validation
 */

// Return whether a validation list has been set
bool EnumOptionsModuleKeyword::hasValidationList()
{
	return true;
}

// Return validation list
const Array<CharString>& EnumOptionsModuleKeyword::validationList()
{
	return validKeywords_;
}

// Validate supplied value
bool EnumOptionsModuleKeyword::isValid(CharString value)
{
	return data_.isValid(value.get());
}

/*
 * Conversion
 */

// Return value (as string)
const char* EnumOptionsModuleKeyword::asString()
{
	return data_.currentOptionKeyword();
}
