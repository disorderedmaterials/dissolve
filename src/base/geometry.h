/*
	*** Geometry
	*** src/base/geometry.h
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

#ifndef DISSOLVE_GEOMETRY_H
#define DISSOLVE_GEOMETRY_H


#include "templates/listitem.h"
#include <map>

// Forward Declarations
/* none */



// Geometry Definition
class Geometry : public ListItem<Geometry>
{
	public:
	// Constructor
	Geometry();
	// Destructor
	~Geometry();
	// Type of geometry data passed
	enum GeometryType { 
		AngleType,
		DistanceType,
		TorsionType
	};


	/*
	 * Data
	 */
	private:
	// Reference value
	double value_;
	// Array for indices
	int indices_[4];
	
	public:
	// Set reference value and indices
	void set(double value, int a, int b, int c = -1, int d = -1);
	// Return reference value
	double value();
	// Return specified index
	int indices(int i) const;
	
};

#endif
