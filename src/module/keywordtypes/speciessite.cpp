/*
	*** Module Keyword - Species Site
	*** src/modules/keywordtypes/speciessite.cpp
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

#include "module/keywordtypes/speciessite.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "genericitems/listhelper.h"

// Constructor
SpeciesSiteModuleKeyword::SpeciesSiteModuleKeyword(SpeciesSite* site) : ModuleKeywordData<SpeciesSite*>(ModuleKeywordBase::SpeciesSiteData, site)
{
}

// Destructor
SpeciesSiteModuleKeyword::~SpeciesSiteModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool SpeciesSiteModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesSiteModuleKeyword::minArguments()
{
	return 2;
}

// Return maximum number of arguments accepted
int SpeciesSiteModuleKeyword::maxArguments()
{
	return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool SpeciesSiteModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	// Find target Species (first argument)
	Species* sp = coreData.findSpecies(parser.argc(startArg));
	if (!sp)
	{
		Messenger::error("Error setting SpeciesSite - no Species named '%s' exists.\n", parser.argc(startArg));
		return false;
	}

	// Find specified Site (second argument) in the Species
	data_ = sp->findSite(parser.argc(startArg+1));
	if (!data_) return Messenger::error("Error setting SpeciesSite - no such site named '%s' exists in Species '%s'.\n", parser.argc(startArg+1), sp->name());

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool SpeciesSiteModuleKeyword::write(LineParser& parser, const char* prefix)
{
	if (data_)
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'\n", prefix, keyword(), data_->parent()->name(), data_->name())) return false;
	}
	else if (!parser.writeLineF("%s%s  '?_?'  '?_?'\n", prefix, keyword())) return false;

	return true;
}
