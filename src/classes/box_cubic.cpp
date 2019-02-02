/*
	*** Cubic Box
	*** src/classes/box_cubic.cpp
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

#include "classes/box.h"
#include "classes/atom.h"
#include "classes/cell.h"

// Constructor
CubicBox::CubicBox(double volume, double boxLength) : Box()
{
	type_ = Box::CubicBoxType;
	
	// Construct axes_
	axes_.setColumn(0, boxLength, 0.0, 0.0);
	axes_.setColumn(1, 0.0, boxLength, 0.0);
	axes_.setColumn(2, 0.0, 0.0, boxLength);
	
	// Set up box, rescaling to desired volume
	setUp(volume);

	// Grab new cell length
	a_ = axes_.columnMagnitude(0);
	ra_ = 1.0/a_;
}

// Destructor
CubicBox::~CubicBox()
{
}

/*
 * Minimum Image Routines (virtual implementations)
 */

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> CubicBox::minimumImage(const Atom* i, const Atom* ref) const
{
	/*
	 * This, and all other routines, use a ternary if/else test and an int() cast in order to calculate minimum image vectors, distances, and coordinates,
	 * since this is about 25-30% faster than using floor().
	 */
	Vec3<double> mimVec = i->r();
	mimVec -= ref->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	mimVec += ref->r();
	return mimVec;
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> CubicBox::minimumImage(const Atom* i, const Vec3<double>& ref) const
{
	Vec3<double> mimVec = i->r();
	mimVec -= ref;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;
	
	mimVec += ref;
	return mimVec;
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> CubicBox::minimumImage(const Vec3<double>& i, const Vec3<double>& ref) const
{
	Vec3<double> mimVec = i;
	mimVec -= ref;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	mimVec += ref;
	return mimVec;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> CubicBox::minimumVector(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> CubicBox::minimumVector(const Atom& i, const Atom& j) const
{
	Vec3<double> mimVec = j.r();
	mimVec -= i.r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> CubicBox::minimumVector(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> CubicBox::minimumVector(const Vec3<double>& i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec;
}

// Return minimum image distance from 'i' to 'j'
double CubicBox::minimumDistance(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitude();
}

// Return minimum image distance from 'i' to 'j'
double CubicBox::minimumDistance(const Atom& i, const Atom& j) const
{
	Vec3<double> mimVec = j.r();
	mimVec -= i.r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitude();
}

// Return minimum image distance from 'i' to 'j'
double CubicBox::minimumDistance(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitude();
}

// Return minimum image distance from 'i' to 'j'
double CubicBox::minimumDistance(const Vec3<double>& i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;
	
	return mimVec.magnitude();
}

// Return minimum image squared distance from 'i' to 'j' (pointers)
double CubicBox::minimumDistanceSquared(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j' (references)
double CubicBox::minimumDistanceSquared(const Atom& i, const Atom& j) const
{
	Vec3<double> mimVec = j.r();
	mimVec -= i.r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double CubicBox::minimumDistanceSquared(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*ra_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.z -= int(mimVec.z*ra_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*a_;

	return mimVec.magnitudeSq();
}

// Return minimum image distance from 'i' to 'j'
double CubicBox::minimumDistanceSquared(const Vec3<double>& i, const Vec3<double>& j) const
{
	double x = j.x-i.x, y = j.y-i.y, z = j.z-i.z;

	x -= int(x*ra_ + (x < 0.0 ? -0.5 : 0.5))*a_;
	y -= int(y*ra_ + (y < 0.0 ? -0.5 : 0.5))*a_;
	z -= int(z*ra_ + (z < 0.0 ? -0.5 : 0.5))*a_;

	return (x*x + y*y + z*z);
}

/*
 * Utility Routines (Virtual Implementations)
 */

// Return random coordinate inside Box
Vec3<double> CubicBox::randomCoordinate() const
{
	Vec3<double> pos;
	pos.x = a_*DissolveMath::random();
	pos.y = a_*DissolveMath::random();
	pos.z = a_*DissolveMath::random();
	return pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> CubicBox::fold(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	static Vec3<double> frac;
	frac.set(r.x*ra_, r.y*ra_, r.z*ra_);
	
	// Fold into Box
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);

	return frac*a_;
}

// Return folded fractional coordinate (i.e. inside current Box)
Vec3<double> CubicBox::foldFrac(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	static Vec3<double> frac;
	frac.set(r.x*ra_, r.y*ra_, r.z*ra_);
	
	// Fold into Box
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);

	return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> CubicBox::fracToReal(const Vec3<double>& r) const
{
	// Multiply by box length
	return r * a_;
}
