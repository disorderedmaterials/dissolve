/*
	*** Coordinate Set
	*** src/classes/coordinateset.h
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

#ifndef DISSOLVE_COORDINATESET_H
#define DISSOLVE_COORDINATESET_H

#include "templates/array.h"
#include "templates/listitem.h"
#include "templates/vector3.h"

// Forward Declarations
/* none */

// Coordinate Set
class CoordinateSet : public ListItem<CoordinateSet>
{
	public:
	// Constructor
	CoordinateSet();
	// Destructor
	~CoordinateSet();


	/*
	 * Coordinates
	 */
	protected:
	// Coordinates
	Array< Vec3<double> > coordinates_;

	public:
	// Initialise to specified size
	void initialise(int size);
	// Return size
	int size() const;
	// Set specified coordinates
	void set(int index, Vec3<double> r);
	// Return specified coordinates
	Vec3<double> r(int index) const;
	// Return coordinates array
	Array< Vec3<double> >& coordinates();
};

#endif

