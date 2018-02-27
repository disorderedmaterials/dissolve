/*
	*** Module Keyword - Broadening Function
	*** src/modules/keywordtypes/broadeningfunction.cpp
	Copyright T. Youngs 2012-2018

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

#include "module/keywordtypes/broadeningfunction.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
BroadeningFunctionModuleKeyword::BroadeningFunctionModuleKeyword(BroadeningFunction value) : ModuleKeywordBase(ModuleKeywordBase::BroadeningFunctionData), ModuleKeywordData<BroadeningFunction>(value)
{
}

// Destructor
BroadeningFunctionModuleKeyword::~BroadeningFunctionModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void BroadeningFunctionModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<BroadeningFunction>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

// Return whether the current data value has ever been set
bool BroadeningFunctionModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int BroadeningFunctionModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int BroadeningFunctionModuleKeyword::maxArguments()
{
	return MAXBROADENINGFUNCTIONPARAMS;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool BroadeningFunctionModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	bool result = data_.set(parser, startArg);
	if (result) set_ = true;

	return result;
}

// Write keyword data to specified LineParser
bool BroadeningFunctionModuleKeyword::write(LineParser& parser, const char* prefix)
{
	CharString params;
	for (int n=0; n<BroadeningFunction::nFunctionParameters(data_.function()); ++n) params.strcatf("  %f", data_.parameter(n));
	return parser.writeLineF("%s%s  '%s'%s\n", prefix, keyword(), BroadeningFunction::functionType(data_.function()), params.get());
}

/*
 * Validation
 */

// Validate supplied value
bool BroadeningFunctionModuleKeyword::isValid(BroadeningFunction value)
{
	return true;
}
