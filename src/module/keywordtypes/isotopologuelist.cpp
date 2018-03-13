/*
	*** Module Keyword - Isotopologue Reference List
	*** src/modules/keywordtypes/isotopologuelist.cpp
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

#include "module/keywordtypes/isotopologuelist.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
IsotopologueListModuleKeyword::IsotopologueListModuleKeyword(List<IsotopologueReference>& references) : ModuleKeywordBase(ModuleKeywordBase::IsotopologueListData), ModuleKeywordData<IsotopologueReference>(IsotopologueReference()), references_(references)
{
}

// Destructor
IsotopologueListModuleKeyword::~IsotopologueListModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void IsotopologueListModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<IsotopologueReference>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

// Return whether the current data value has ever been set
bool IsotopologueListModuleKeyword::isSet()
{
	return set_;
}

// Return list of references
List<IsotopologueReference>& IsotopologueListModuleKeyword::references()
{
	return references_;
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

// Parse arguments from supplied LineParser, starting at argument offset specified
bool IsotopologueListModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	// Find target Configuration (first argument)
	Configuration* cfg = NULL;
	for (cfg = List<Configuration>::masterInstance().first(); cfg != NULL; cfg = cfg->next) if (DUQSys::sameString(parser.argc(startArg), cfg->name())) break;
	if (!cfg)
	{
		Messenger::error("Error defining Isotopologue reference - no Configuration named '%s' exists.\n", parser.argc(startArg));
		return false;
	}

	// Find specified Species (second argument) - must be present in the target Configuration
	Species* sp = NULL;
	for (sp = List<Species>::masterInstance().first(); sp != NULL; sp = sp->next) if (DUQSys::sameString(parser.argc(startArg+1), sp->name())) break;
	if (!sp) return Messenger::error("Error defining Isotopologue reference - no Species named '%s' exists.\n", parser.argc(startArg+1));
	if (!cfg->hasUsedSpecies(sp))return Messenger::error("Error defining Isotopologue reference - Species '%s' is not present in Configuration '%s'.\n", sp->name(), cfg->name());

	// Finally, locate isotopologue definition for species
	Isotopologue* iso = sp->findIsotopologue(parser.argc(startArg+2));
	if (!iso) return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(startArg+2), sp->name());

	// Add the data to the list
	IsotopologueReference* isoRef = references_.add();
	isoRef->set(cfg, sp, iso, parser.argd(startArg+3));
	
	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool IsotopologueListModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of IsotopologueReferences
	ListIterator<IsotopologueReference> refIterator(references_);
	while (IsotopologueReference* ref = refIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, keyword(), ref->configuration()->name(), ref->species()->name(), ref->isotopologue()->name(), ref->weight())) return false;
	}

	return true;
}

/*
 * Validation
 */

// Validate supplied value
bool IsotopologueListModuleKeyword::isValid(IsotopologueReference value)
{
	return true;
}
