/*
	*** Calculate Distance-Angle Module - Functions
	*** src/modules/calculate/dangle/functions.cpp
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

#include "modules/calculate/dangle/dangle.h"

// Return name of B..C RDF
const char* CalculateDAngleModule::rdfBCResultName() const
{
	static CharString result;
	result = keywords_.asString("Name");
	if (result.isEmpty()) result = uniqueName();

	result += "_RDF(BC)";

	return result.get();
}

// Return name of A-B...C angle distribution
const char* CalculateDAngleModule::angleABCResultName() const
{
	static CharString result;
	result = keywords_.asString("Name");
	if (result.isEmpty()) result = uniqueName();

	result += "_ANGLE(ABC)";

	return result.get();
}

// Return name of resulting distance-angle map
const char* CalculateDAngleModule::resultName() const
{
	static CharString result;
	result = keywords_.asString("Name");
	if (result.isEmpty()) result = uniqueName();

	return result.get();
}
