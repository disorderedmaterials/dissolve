/*
	*** Keyword - EnumOptions
	*** src/keywords/enumoptions.cpp
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

// Constructor
EnumOptionsKeyword::EnumOptionsKeyword(EnumOptionsBase options) : KeywordData<EnumOptionsBase>(KeywordBase::EnumOptionsData, options)
{
	// Set our array of valid values
	for (int n=0; n<data_.nOptions(); ++n) validKeywords_.add(data_.keywordByIndex(n));
}

// Destructor
EnumOptionsKeyword::~EnumOptionsKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int EnumOptionsKeyword::minArguments() const
{
	return 1;
}

// Return minimum number of arguments accepted
int EnumOptionsKeyword::maxArguments() const
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool EnumOptionsKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
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
bool EnumOptionsKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	return parser.writeLineF("%s%s  '%s'\n", prefix, keyword(), data_.currentOptionKeyword());
}

/*
 * Validation
 */

// Return whether a validation list has been set
bool EnumOptionsKeyword::hasValidationList()
{
	return true;
}

// Return validation list
const Array<CharString>& EnumOptionsKeyword::validationList()
{
	return validKeywords_;
}

// Validate supplied value
bool EnumOptionsKeyword::isValid(CharString value)
{
	return data_.isValid(value.get());
}

/*
 * Conversion
 */

// Return value (as string)
const char* EnumOptionsKeyword::asString()
{
	return data_.currentOptionKeyword();
}
