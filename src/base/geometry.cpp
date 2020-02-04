/*
	*** Geometry
	*** src/base/geometry.cpp
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

#include "base/geometry.h"

// Forward Declarations
/* none */

// Constructor
Geometry::Geometry()
{
	indices_[0] = -1;
	indices_[1] = -1;
	indices_[2] = -1;
	indices_[3] = -1;
	value_ = 0.0;
}

// Destructor
Geometry::~Geometry()
{
}

/*
 * Data
 */
	
// Set reference value and indices
void Geometry::set(double value, int a, int b, int c, int d)
{
	indices_[0] = a;
	indices_[1] = b;
	indices_[2] = c;
	indices_[3] = d;
	value_ = value;
}

// Return reference value
double Geometry::value()
{
	return value_;
}

// Return index value
int Geometry::indices(int i) const
{
	return indices_[i];
}


