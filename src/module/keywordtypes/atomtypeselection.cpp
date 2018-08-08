/*
	*** Module Keyword - AtomType Selection
	*** src/modules/keywordtypes/atomtypeselection.cpp
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

#include "module/keywordtypes/atomtypeselection.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/configuration.h"
#include "base/lineparser.h"

// Constructor
AtomTypeSelectionModuleKeyword::AtomTypeSelectionModuleKeyword(AtomTypeList& selection, RefList<Configuration,bool>& sourceConfigurations) : ModuleKeywordBase(ModuleKeywordBase::AtomTypeSelectionData), ModuleKeywordData<AtomTypeList&>(selection), sourceConfigurations_(sourceConfigurations)
{
}

// Destructor
AtomTypeSelectionModuleKeyword::~AtomTypeSelectionModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool AtomTypeSelectionModuleKeyword::isSet()
{
	return set_;
}

// Return list of AtomTpe/bool references
AtomTypeList& AtomTypeSelectionModuleKeyword::selection()
{
	// Update the list first, in case a Configuration has changed
	checkSelection();

	return data_;
}

// Check AtomType selection and make sure it is consistent based on the source Configurations
void AtomTypeSelectionModuleKeyword::checkSelection()
{
	AtomTypeList newSelection;

	// Loop over existing selection, checking for each AtomType existing in any source Configuration
	ListIterator<AtomTypeData> typeIterator(data_.types());
	while (AtomTypeData* atd = typeIterator.iterate())
	{
		bool found = false;
		RefListIterator<Configuration,bool> cfgIterator(sourceConfigurations_);
		while (Configuration* cfg = cfgIterator.iterate())
		{
			if (cfg->usedAtomTypesList().contains(atd->atomType()))
			{
				found = true;
				break;
			}
		}

		if (found) newSelection.add(atd->atomType());
	}

	// Copy the new list over the old one
	data_ = newSelection;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeSelectionModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int AtomTypeSelectionModuleKeyword::maxArguments()
{
	return 999;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool AtomTypeSelectionModuleKeyword::parseArguments(LineParser& parser, int startArg, ProcessPool& procPool)
{
	// Make sure our list is up-to-date
	checkSelection();

	// Loop over arguments (which are AtomType names) and add them to our list
	for (int n = startArg; n < parser.nArgs(); ++n)
	{
		// Do we recognise the AtomType?
		AtomType* atomType = NULL;
		ListIterator<AtomType> typeIterator(List<AtomType>::masterInstance());
		while (atomType = typeIterator.iterate()) if (DissolveSys::sameString(atomType->name(), parser.argc(n))) break;
		if (!atomType) return Messenger::error("Unrecognised AtomType '%s' found in list.\n", parser.argc(n));

		// If the AtomType is in the list already, complain
		if (data_.contains(atomType)) return Messenger::error("AtomType '%s' specified in selection list twice.\n", parser.argc(n));

		// All OK - add it to our selection list
		data_.add(atomType);
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool AtomTypeSelectionModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over the AtomType selection list
	CharString selection;
	ListIterator<AtomTypeData> typeIterator(data_.types());
	while (AtomTypeData* atd = typeIterator.iterate()) selection.strcatf("  %s", atd->atomTypeName());

	if (!parser.writeLineF("%s%s%s", prefix, keyword(), selection.get())) return false;

	return true;
}
