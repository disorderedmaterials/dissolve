/*
	*** Orthogonal Cuboid Class
	*** src/math/cuboid.cpp
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

#include "math/mathfunc.h"
#include "math/cuboid.h"

// Constructor
Cuboid::Cuboid()
{
	minima_.zero();
	maxima_.zero();
	minimaSet_ = false;
	maximaSet_ = false;
}

/*
 * Definition
 */

// Update extreme coordinates with supplied vector
void Cuboid::updateExtremes(Vec3<double> v)
{
	if (!minimaSet_) minima_ = v;
	else if (v.x < minima_.x) minima_.x = v.x;
	else if (v.y < minima_.y) minima_.y = v.y;
	else if (v.z < minima_.z) minima_.z = v.z;

	if (!maximaSet_) maxima_ = v;
	else if (v.x > maxima_.x) maxima_.x = v.x;
	else if (v.y > maxima_.y) maxima_.y = v.y;
	else if (v.z > maxima_.z) maxima_.z = v.z;

	minimaSet_ = true;
	maximaSet_ = true;
}

/*
 * Functions
 */

// Return minima for cuboid
Vec3<double> Cuboid::minima() const
{
	return minima_;
}

// Return maxima for cuboid
Vec3<double> Cuboid::maxima() const
{
	return maxima_;
}

// Return whether 2D coordinates are within projection of cuboid
bool Cuboid::isPointWithinProjection(int x, int y, Matrix4 viewMatrix, Matrix4 projectionMatrix, const GLuint* viewPort)
{
// 	Vec3<double> a, b, c, d;
// 	double rectArea, testArea;
	// XY plane at maximum Z
// 	a = UChromaMath::glModelToScreen(Vec3<double>(minima_.x, minima_.y, minima_.z), viewMatrix, projectionMatrix, viewPort);
// 	b.set(maxima_.x, minima_.y, minima_.z);
// 	c.set(maxima_.x, maxima_.y, minima_.z);
// 	d.set(minima_.x, maxima_.y, minima_.z);
// 	rectArea = UChromaMath::triangleArea(a.) + 
	return false;
}
