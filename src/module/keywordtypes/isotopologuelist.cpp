/*
	*** Module Keyword - Isotopologue Reference List
	*** src/modules/keywordtypes/isotopologuelist.cpp
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

#include "module/keywordtypes/isotopologuelist.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
IsotopologueListModuleKeyword::IsotopologueListModuleKeyword(List<IsotopologueReference>& references) : ModuleKeywordBase(ModuleKeywordBase::IsotopologueListData), ModuleKeywordData< List<IsotopologueReference>& >(references)
{
}

// Destructor
IsotopologueListModuleKeyword::~IsotopologueListModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool IsotopologueListModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IsotopologueListModuleKeyword::minArguments()
{
	return 4;
}

// Return maximum number of arguments accepted
int IsotopologueListModuleKeyword::maxArguments()
{
	return 4;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool IsotopologueListModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	// Find target Configuration (first argument)
	Configuration* cfg = coreData.findConfiguration(parser.argc(startArg));
	if (!cfg)
	{
		Messenger::error("Error defining Isotopologue reference - no Configuration named '%s' exists.\n", parser.argc(startArg));
		return false;
	}

	// Find specified Species (second argument) - must be present in the target Configuration
	Species* sp = coreData.findSpecies(parser.argc(startArg+1));
	if (!sp) return Messenger::error("Error defining Isotopologue reference - no Species named '%s' exists.\n", parser.argc(startArg+1));
	if (!cfg->hasUsedSpecies(sp))return Messenger::error("Error defining Isotopologue reference - Species '%s' is not present in Configuration '%s'.\n", sp->name(), cfg->name());

	// Finally, locate isotopologue definition for species
	Isotopologue* iso = sp->findIsotopologue(parser.argc(startArg+2));
	if (!iso) return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(startArg+2), sp->name());

	// Add the data to the list
	IsotopologueReference* isoRef = data_.add();
	isoRef->set(cfg, sp, iso, parser.argd(startArg+3));
	
	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool IsotopologueListModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of IsotopologueReferences
	ListIterator<IsotopologueReference> refIterator(data_);
	while (IsotopologueReference* ref = refIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, keyword(), ref->configuration()->name(), ref->species()->name(), ref->isotopologue()->name(), ref->weight())) return false;
	}

	return true;
}
