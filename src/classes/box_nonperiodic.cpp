/*
	*** Non-Periodic (Cubic) Box
	*** src/classes/box_nonperiodic.cpp
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
#include "templates/array.h"

// Constructor
NonPeriodicBox::NonPeriodicBox(double volume) : Box()
{
	type_ = Box::NonPeriodicBoxType;
	periodic_.set(false, false, false);

	// Construct axes_
	axes_.setColumn(0, 1.0, 0.0, 0.0);
	axes_.setColumn(1, 0.0, 1.0, 0.0);
	axes_.setColumn(2, 0.0, 0.0, 1.0);

	// Set up box, rescaling to desired volume
	setUp(volume);

	// Grab new cell length
	a_ = axes_.columnMagnitude(0);
	ra_ = 1.0/a_;
}

// Destructor
NonPeriodicBox::~NonPeriodicBox()
{
}

/*
 * Minimum Image Routines (virtual implementations)
*/

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> NonPeriodicBox::minimumImage(const Atom* i, const Atom* ref) const
{
	return i->r();
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> NonPeriodicBox::minimumImage(const Atom* i, const Vec3<double>& ref) const
{
	return i->r();
}

// Return minimum image coordinates of 'i' with respect to 'ref'
Vec3<double> NonPeriodicBox::minimumImage(const Vec3<double>& i, const Vec3<double>& ref) const
{
	return i;
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> NonPeriodicBox::minimumVector(const Atom* i, const Atom* j) const
{
	return j->r() - i->r();
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> NonPeriodicBox::minimumVector(const Atom& i, const Atom& j) const
{
	return j.r() - i.r();
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> NonPeriodicBox::minimumVector(const Atom* i, const Vec3<double>& j) const
{
	return j - i->r();
}

// Return minimum image vector from 'i' to 'j'
Vec3<double> NonPeriodicBox::minimumVector(const Vec3<double>& i, const Vec3<double>& j) const
{
	return j - i;
}

// Return minimum image distance from 'i' to 'j'
double NonPeriodicBox::minimumDistance(const Atom* i, const Atom* j) const
{
	return (j->r() - i->r()).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double NonPeriodicBox::minimumDistance(const Atom& i, const Atom& j) const
{
	return (j.r() - i.r()).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double NonPeriodicBox::minimumDistance(const Atom* i, const Vec3<double>& j) const
{
	return (j - i->r()).magnitude();
}

// Return minimum image distance from 'i' to 'j'
double NonPeriodicBox::minimumDistance(const Vec3<double>& i, const Vec3<double>& j) const
{
	return (j - i).magnitude();
}

// Return minimum image squared distance from 'i' to 'j' (pointers)
double NonPeriodicBox::minimumDistanceSquared(const Atom* i, const Atom* j) const
{
	return (j->r() - i->r()).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j' (references)
double NonPeriodicBox::minimumDistanceSquared(const Atom& i, const Atom& j) const
{
	return (j.r() - i.r()).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double NonPeriodicBox::minimumDistanceSquared(const Atom* i, const Vec3<double>& j) const
{
	return (j - i->r()).magnitudeSq();
}

// Return minimum image squared distance from 'i' to 'j'
double NonPeriodicBox::minimumDistanceSquared(const Vec3<double>& i, const Vec3<double>& j) const
{
	return (j - i).magnitudeSq();
}

/*
 * Utility Routines (Virtual Implementations)
*/

// Return random coordinate inside Box
Vec3<double> NonPeriodicBox::randomCoordinate() const
{
	static Vec3<double> pos;
	pos.x = a_*DissolveMath::random();
	pos.y = a_*DissolveMath::random();
	pos.z = a_*DissolveMath::random();
	return pos;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> NonPeriodicBox::fold(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	static Vec3<double> frac;
	frac.set(r.x*ra_, r.y*ra_, r.z*ra_);
	
	// Fold into Box and divide by integer Cell sizes
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);
	return frac*a_;
}

// Return folded coordinate (i.e. inside current Box)
Vec3<double> NonPeriodicBox::foldFrac(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	static Vec3<double> frac;
	frac.set(r.x*ra_, r.y*ra_, r.z*ra_);
	
	// Fold into Box and divide by integer Cell sizes
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);
	return frac;
}

// Convert supplied fractional coordinates to real space
Vec3<double> NonPeriodicBox::fracToReal(const Vec3<double>& r) const
{
	// Multiply by box length
	return r * a_;
}
