/*
	*** Geometry
	*** src/base/geometry.cpp
	Copyright T. Youngs 2012-2018

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
	a_ = -1;
	b_ = -1;
	c_ = -1;
	d_ = -1;
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
	a_ = a;
	b_ = b;
	c_ = c;
	d_ = d;
	value_ = value;
}

// Return index 1
int Geometry::a()
{
	return a_;
}

// Return index 2
int Geometry::b()
{
	return b_;
}

// Return index 3
int Geometry::c()
{
	return c_;
}

// Return index 4
int Geometry::d()
{
	return d_;
}

// Return reference value
double Geometry::value()
{
	return value_;
}


/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool Geometry::broadcast(ProcessPool& procPool, int root)
{
	return false;
}

