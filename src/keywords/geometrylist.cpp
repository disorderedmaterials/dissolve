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
#include "classes/coredata.h"
#include "base/lineparser.h"

// Constructorist<Geometry>& data
GeometryKeyword:: GeometryKeyword::GeometryKeyword(List<Geometry>& data, Geometry::GeometryType type) : KeywordData<List<Geometry>& >(KeywordBase::GeometryListData, data),
type_(type)
{
}
 
// Destructor
GeometryKeyword::~GeometryKeyword()
{
}

// Return minimum number of arguments accepted
int GeometryKeyword::minArguments() const
{	
	if (type_ == Geometry::GeometryType::Distance)
		return 3;
	
	else if (type_ == Geometry::GeometryType::Angle)
		return 4;
	
	else
		return 5;
}

// Return maximum number of arguments accepted
int GeometryKeyword::maxArguments() const
{
	if (type_ == Geometry::GeometryType::Distance)
		return 3;
	
	else if (type_ == Geometry::GeometryType::Angle)
		return 4;
	
	else
		return 5;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool GeometryKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	totalArgs_ = parser.nArgs();
	Geometry g;
	if (totalArgs_-1 != maxArguments())
	{
		Messenger::error("Error finding '%s' geometry data type", parser.argc(startArg));
		return false;
	}
	if (parser.argd(maxArguments()) <=0)
	{
		Messenger::error("Values '%d' not correct for keyword",parser.argd(maxArguments()));
		return false;
	}
	
	
	if (maxArguments() == 3)
		g.set(parser.argd(2+startArg), parser.argi(0+startArg), parser.argi(1+startArg) );
	
	else if (maxArguments() == 4)
		g.set(parser.argd(3+startArg), parser.argi(startArg), parser.argi(1+startArg), parser.argi(2+startArg));
	else
		g.set(parser.argd(4+startArg), parser.argi(startArg), parser.argi(1+startArg), parser.argi(2+startArg), parser.argi(3+startArg));
	
	return true;
}

// Write keyword data to specified LineParser

bool GeometryKeyword::write(LineParser& parser, const char* keywordName, const char* prefix)
{
	Geometry* g = data_.add();
	CharString index;
	for (int n=0; n<maxArguments()-1; ++n) index.strcatf("  %i", g->indices(n));
	ListIterator<Geometry> GeoIterator(data_);
	while (Geometry* ref = GeoIterator.iterate())
	{

		if (!parser.writeLineF("%s%s%s  %d\n", prefix, keywordName, index.get(), g->value())) 
		return false;
	}

	return true;
}


