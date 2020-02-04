/*
	*** Keyword - AtomType RefList
	*** src/keywords/atomtypereflist.cpp
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

#include "keywords/atomtypereflist.h"
#include "classes/atomtype.h"
#include "classes/coredata.h"
#include "base/lineparser.h"

// Constructor
AtomTypeRefListKeyword::AtomTypeRefListKeyword(RefList<AtomType>& targetRefList) : KeywordData< RefList<AtomType>& >(KeywordBase::AtomTypeRefListData, targetRefList)
{
}

// Destructor
AtomTypeRefListKeyword::~AtomTypeRefListKeyword()
{
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int AtomTypeRefListKeyword::minArguments() const
{
	return 1;
}

// Return maximum number of arguments accepted
int AtomTypeRefListKeyword::maxArguments() const
{
	return 999;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool AtomTypeRefListKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	// Loop over arguments (which are AtomType names) and add them to our list
	for (int n = startArg; n < parser.nArgs(); ++n)
	{
		// Do we recognise the AtomType?
		AtomType* atomType = coreData.findAtomType(parser.argc(n));
		if (!atomType) return Messenger::error("Unrecognised AtomType '%s' found in list.\n", parser.argc(n));

		// If the AtomType is in the list already, complain
		if (data_.contains(atomType)) return Messenger::error("AtomType '%s' specified in list twice.\n", parser.argc(n));

		// All OK - add it to our selection list
		data_.append(atomType);
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool AtomTypeRefListKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	// Don't write anything if there are no items in the list
	if (data_.nItems() == 0) return true;

	// Loop over the AtomType selection list
	CharString atomTypes;
	for(auto at : data_) atomTypes.strcatf("  %s", at->name());

	if (!parser.writeLineF("%s%s%s\n", prefix, keywordName, atomTypes.get())) return false;

	return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeRefListKeyword::removeReferencesTo(AtomType* at)
{
	data_.remove(at);
}
