/*
	*** Keyword - Isotopologue Reference List
	*** src/keywords/isotopologuereferencelist.cpp
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

#include "keywords/geometrylist.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "base/lineparser.h"

// Constructor
GeometryListKeyword:: GeometryListKeyword::GeometryListKeyword(List<Geometry>& data): KeywordData<List<Geometry>& >(KeywordBase::GeometryListData, data),
{
}
 

// Destructor
GeometryListKeyword::~GeometryListKeyword()
{
}



/*
 * Arguments
 */

// Return minimum number of arguments accepted
int GeometryListKeyword::minArguments() const
{
	return 2;
}

// Return maximum number of arguments accepted
int GeometryListKeyword::maxArguments() const
{
	return 3;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool GeometryListKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	
}

// Write keyword data to specified LineParser
bool GeometryListKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of IsotopologueReferences
	ListIterator<IsotopologueReference> refIterator(data_);
	while (IsotopologueReference* ref = refIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, name(), ref->configuration()->name(), ref->species()->name(), ref->isotopologue()->name(), ref->weight())) return false;
	}

	return true;
}


