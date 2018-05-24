/*
	*** Geometry
	*** src/base/geometry.h
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

#ifndef DISSOLVE_GEOMETRY_H
#define DISSOLVE_GEOMETRY_H

#include "templates/mpilistitem.h"

// Forward Declarations
/* none */

// Geometry Definition
class Geometry : public MPIListItem<Geometry>
{
	public:
	// Constructor
	Geometry();
	// Destructor
	~Geometry();


	/*
	 * Data
	 */
	private:
	// Index 1
	int a_;
	// Index 2
	int b_;
	// Index 3
	int c_;
	// Index 4
	int d_;
	// Reference value
	double value_;
	
	public:
	// Set reference value and indices
	void set(double value, int a, int b, int c = -1, int d = -1);
	// Return index 1
	int a();
	// Return index 2
	int b();
	// Return index 3
	int c();
	// Return index 4
	int d();
	// Return reference value
	double value();


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, int root = 0);
};

#endif
