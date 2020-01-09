/*
	*** Coordinate Set
	*** src/classes/coordinateset.cpp
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

#include "classes/coordinateset.h"
#include "base/messenger.h"

// Constructor
CoordinateSet::CoordinateSet() : ListItem<CoordinateSet>()
{
}

// Destructor
CoordinateSet::~CoordinateSet()
{
}

/*
 * Coordinates
 */

// Initialise to specified size
void CoordinateSet::initialise(int size)
{
	coordinates_.initialise(size);
}

// Return size
int CoordinateSet::size() const
{
	return coordinates_.nItems();
}

// Set specified coordinates
void CoordinateSet::set(int index, Vec3<double> r)
{
	coordinates_.at(index) = r;
}

// Return specified coordinates
Vec3<double> CoordinateSet::r(int index) const
{
	return coordinates_.constAt(index);
}

// Return coordinates array
Array< Vec3<double> >& CoordinateSet::coordinates()
{
	return coordinates_;
}
