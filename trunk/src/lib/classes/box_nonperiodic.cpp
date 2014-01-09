/*
	*** Non-Periodic (Cubic) Box
	*** src/lib/classes/box_nonperiodic.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/box.h"
#include "classes/atom.h"
#include "classes/cell.h"
#include "templates/array.h"

/*!
 * \brief Constructor
 * \details Constructor for Cubic Box. 
 */
NonPeriodicBox::NonPeriodicBox(double volume) : Box()
{
	type_ = Box::NonPeriodicBox;
	periodic_.set(false, false, false);

	// Construct axes_
	axes_.setColumn(0, 1.0, 0.0, 0.0);
	axes_.setColumn(1, 0.0, 1.0, 0.0);
	axes_.setColumn(2, 0.0, 0.0, 1.0);
	
	// Rescale to desired volume
	setVolume(volume);

	// Grab new cell length
	a_ = axes_.columnMagnitude(0);
	ra_ = 1.0/a_;
	
	// Calculate inverse
	inverseAxes_ = axes_;
	inverseAxes_.invert();
}

/*!
 * \brief Destructor
 * \details Constructor for NonPeriodicBox.
 */
NonPeriodicBox::~NonPeriodicBox()
{
}

/*
// Minimum Image Routines (virtual implementations)
*/

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> NonPeriodicBox::minimumImage(const Atom* i, const Atom* ref) const
{
	return i->r();
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> NonPeriodicBox::minimumImage(const Atom* i, const Vec3<double>& ref) const
{
	return i->r();
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> NonPeriodicBox::minimumImage(const Vec3<double>& i, const Vec3<double>& ref) const
{
	return i;
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> NonPeriodicBox::minimumVector(const Atom* i, const Atom* j) const
{
	return j->r() - i->r();
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> NonPeriodicBox::minimumVector(const Atom* i, const Vec3<double>& j) const
{
	return j - i->r();
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> NonPeriodicBox::minimumVector(const Vec3<double>& i, const Vec3<double>& j) const
{
	return j - i;
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double NonPeriodicBox::minimumDistance(const Atom* i, const Atom* j) const
{
	return (j->r() - i->r()).magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double NonPeriodicBox::minimumDistance(const Atom* i, const Vec3<double>& j) const
{
	return (j - i->r()).magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double NonPeriodicBox::minimumDistance(const Vec3<double>& i, const Vec3<double>& j) const
{
	return (j - i).magnitude();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j' (pointers)
 */
double NonPeriodicBox::minimumDistanceSquared(const Atom* i, const Atom* j) const
{
	return (j->r() - i->r()).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j' (references)
 */
double NonPeriodicBox::minimumDistanceSquared(const Atom& i, const Atom& j) const
{
	return (j.r() - i.r()).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double NonPeriodicBox::minimumDistanceSquared(const Atom* i, const Vec3<double>& j) const
{
	return (j - i->r()).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double NonPeriodicBox::minimumDistanceSquared(const Vec3<double>& i, const Vec3<double>& j) const
{
	return (j - i).magnitudeSq();
}

/*
// Utility Routines (Virtual Implementations)
*/

/*!
 * \brief Return random coordinate inside Box
 */
Vec3<double> NonPeriodicBox::randomCoordinate() const
{
	static Vec3<double> pos;
	pos.x = a_*DUQMath::random();
	pos.y = a_*DUQMath::random();
	pos.z = a_*DUQMath::random();
	return pos;
}

/*!
 * \brief Return folded coordinate (i.e. inside current Box)
 */
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

/*!
 * \brief Return folded coordinate (i.e. inside current Box)
 */
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
