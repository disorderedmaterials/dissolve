/*
	*** Keyword - AtomType Selection
	*** src/keywords/atomtypeselection.cpp
	Copyright T. Youngs 2012-2020

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

#include "keywords/atomtypeselection.h"
#include "classes/atomtype.h"
#include "classes/atomtypelist.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "base/lineparser.h"

// Constructor
AtomTypeSelectionKeyword::AtomTypeSelectionKeyword(AtomTypeList& selection, const RefList<Configuration>& sourceConfigurations) : KeywordData<AtomTypeList&>(KeywordBase::AtomTypeSelectionData, selection), sourceConfigurations_(sourceConfigurations)
{
}

// Destructor
AtomTypeSelectionKeyword::~AtomTypeSelectionKeyword()
{
}

/*
 * Data
 */

// Determine whether current data is 'empty', and should be considered as 'not set'
bool AtomTypeSelectionKeyword::isDataEmpty() const
{
	return data_.nItems() == 0;
}

// Check AtomType selection and make sure it is consistent based on the source Configurations
void AtomTypeSelectionKeyword::checkSelection()
{
	AtomTypeList newSelection;

	// Loop over existing selection, checking for each AtomType existing in any source Configuration
	ListIterator<AtomTypeData> typeIterator(data_.types());
	while (AtomTypeData* atd = typeIterator.iterate())
	{
		bool found = false;
		for(auto cfg : sourceConfigurations_)
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

// Return list of AtomTpe/bool references
AtomTypeList& AtomTypeSelectionKeyword::selection()
{
	// Update the list first, in case a Configuration has changed
	checkSelection();

	return data_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeSelectionKeyword::minArguments() const
{
	return 1;
}

// Return maximum number of arguments accepted
int AtomTypeSelectionKeyword::maxArguments() const
{
	return 999;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool AtomTypeSelectionKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	// Make sure our list is up-to-date
	checkSelection();

	// Loop over arguments (which are AtomType names) and add them to our list
	for (int n = startArg; n < parser.nArgs(); ++n)
	{
		// Do we recognise the AtomType?
		AtomType* atomType = NULL;
		ListIterator<AtomType> typeIterator(coreData.constAtomTypes());
		while ((atomType = typeIterator.iterate())) if (DissolveSys::sameString(atomType->name(), parser.argc(n))) break;
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
bool AtomTypeSelectionKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	// Loop over the AtomType selection list
	CharString selection;
	ListIterator<AtomTypeData> typeIterator(data_.types());
	while (AtomTypeData* atd = typeIterator.iterate()) selection.strcatf("  %s", atd->atomTypeName());

	if (!parser.writeLineF("%s%s%s\n", prefix, keywordName, selection.get())) return false;

	return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeSelectionKeyword::removeReferencesTo(AtomType* at)
{
	data_.remove(at);
}
