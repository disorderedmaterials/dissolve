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
GeometryListKeyword:: GeometryListKeyword::GeometryListKeyword(List<Geometry>& data, Geometry::GeometryType type) : KeywordData<List<Geometry>& >(KeywordBase::GeometryListData, data),
type_(type)
{
}
 
// Destructor
GeometryListKeyword::~GeometryListKeyword()
{
}

// Return minimum number of arguments accepted
int GeometryListKeyword::minArguments() const
{	
	if (type_ == Geometry::GeometryType::Distance)
		return 3;
	
	else if (type_ == Geometry::GeometryType::Angle)
		return 4;
	
	else
		return 5;
}

// Return maximum number of arguments accepted
int GeometryListKeyword::maxArguments() const
{
	if (type_ == Geometry::GeometryType::Distance)
		return 3;
	
	else if (type_ == Geometry::GeometryType::Angle)
		return 4;
	
	else
		return 5;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool GeometryListKeyword::read(LineParser& parser, int startArg, const CoreData& coreData)
{
	totalArgs_ = parser.nArgs();
	if (totalArgs_-1 != maxArguments())
	{
		Messenger::error("Error finding '%s' geometry data type", parser.argc(startArg));
		return false;
	}
	if (parser.argd(maxArguments()-1) <=0)
	{
		Messenger::error("Values '%d' not correct for keyword",parser.argd(maxArguments()-1));
		return false;
	}
	return true;
}

// Write keyword data to specified LineParser
bool GeometryListKeyword::write(LineParser& parser, const char* prefix)

{
	/*
	//print enum type;
	parser.writeLineF("'%s'",type_);
	
	
	ListIterator<Geometry> GeoIterator(data_);
	while (Geometry* ref = GeoIterator.iterate())
	{
		
		parser.writeLineF("'%i'", 
		
	}
	*/
	
	if (type_ == Geometry::GeometryType::Distance)
		parser.writeLineF("%sDistance  %i  %i  %d", prefix, parser.argi(1), parser.argi(2), parser.argd(3));
	
	else if (type_ == Geometry::GeometryType::Angle)
		parser.writeLineF("%sAngle  %i  %i  %i  %d", prefix, parser.argi(1), parser.argi(2), parser.argi(3), parser.argd(4));  
	
	else
		parser.writeLineF("%sTorsion  %i  %i  %i  %i  %d", prefix, parser.argi(1), parser.argi(2), parser.argi(3), parser.argi(4), parser.argd(5)); 
		
	return true;
}


