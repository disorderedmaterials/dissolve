/*
	*** Module Keyword - Analyser
	*** src/modules/keywordtypes/analyser.cpp
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

#include "module/keywordtypes/analyser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
AnalyserModuleKeyword::AnalyserModuleKeyword(Analyser& analyser) : ModuleKeywordBase(ModuleKeywordBase::AnalyserData), ModuleKeywordData<Analyser&>(analyser)
{
}

// Destructor
AnalyserModuleKeyword::~AnalyserModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool AnalyserModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AnalyserModuleKeyword::minArguments()
{
	return 0;
}

// Return maximum number of arguments accepted
int AnalyserModuleKeyword::maxArguments()
{
	return 0;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool AnalyserModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (!data_.read(parser, coreData)) return false;

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool AnalyserModuleKeyword::write(LineParser& parser, const char* prefix)
{
	return data_.write(parser, prefix);
}
