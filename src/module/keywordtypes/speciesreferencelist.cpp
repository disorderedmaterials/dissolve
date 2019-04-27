/*
	*** Module Keyword - Species Reference List
	*** src/modules/keywordtypes/speciesreferencelist.cpp
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

#include "module/keywordtypes/speciesreferencelist.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"

// Constructor
SpeciesReferenceListModuleKeyword::SpeciesReferenceListModuleKeyword(RefList<Species,bool>& references) : ModuleKeywordBase(ModuleKeywordBase::SpeciesReferenceListData), ModuleKeywordData< RefList<Species,bool>& >(references)
{
}

// Destructor
SpeciesReferenceListModuleKeyword::~SpeciesReferenceListModuleKeyword()
{
}

/*
 * Data
 */

// Determine whether current data is actually 'set'
bool SpeciesReferenceListModuleKeyword::currentDataIsSet() const
{
	return data_.nItems() > 0;
}

// Return whether the current data value has ever been set
bool SpeciesReferenceListModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesReferenceListModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int SpeciesReferenceListModuleKeyword::maxArguments()
{
	return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool SpeciesReferenceListModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	// Each argument is the name of a Species that we will add to our list
	for (int n=startArg; n < parser.nArgs(); ++n)
	{
		Species* sp = coreData.findSpecies(parser.argc(n));
		if (!sp) return Messenger::error("Error defining Species targets - no Species named '%s' exists.\n", parser.argc(n));

		data_.add(sp);
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool SpeciesReferenceListModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of Species
	CharString speciesString;
	RefListIterator<Species,bool> speciesIterator(data_);
	while (Species* sp = speciesIterator.iterate()) speciesString.strcatf("  %s", sp->name());

	if (!parser.writeLineF("%s%s  %s\n", prefix, keyword(), speciesString.get())) return false;

	return true;
}
