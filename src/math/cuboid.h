/*
	*** Orthogonal Cuboid Class
	*** src/math/cuboid.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_CUBOID_H
#define DISSOLVE_CUBOID_H

#include "math/matrix4.h"

// Orthogonal Cuboid
class Cuboid
{
	public:
	// Constructor / Destructor
	Cuboid();


	/*
	 * Definition
	 */
	private:
	// Minimum and maximum coordinates for cuboid
	Vec3<double> minima_, maxima_;
	// Whether minima and maxima have ever been set
	bool minimaSet_, maximaSet_;

	public:
	// Update extreme coordinates with supplied vector
	void updateExtremes(Vec3<double> v);


	/*
	 * Functions
	 */
	public:
	// Return minima for cuboid
	Vec3<double> minima() const;
	// Return maxima for cuboid
	Vec3<double> maxima() const;
};

#endif

