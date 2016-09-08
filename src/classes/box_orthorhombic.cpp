/*
	*** Orthorhombic Box
	*** src/classes/box_orthorhombic.cpp
	Copyright T. Youngs 2012-2014

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

/*!
 * \brief Constructor
 * \details Constructor for Orthorhombic Box. 
 */
OrthorhombicBox::OrthorhombicBox(double volume, const Vec3<double> relativeLengths) : Box()
{
	type_ = Box::OrthorhombicBox;
	
	// Construct axes_
	axes_.setColumn(0, relativeLengths.x, 0.0, 0.0);
	axes_.setColumn(1, 0.0, relativeLengths.y, 0.0);
	axes_.setColumn(2, 0.0, 0.0, relativeLengths.z);
	
	// Setup box, rescaling to desired volume
	setup(volume);
	
	// Grab new cell lengths
	a_ = axes_.columnMagnitude(0);
	b_ = axes_.columnMagnitude(1);
	c_ = axes_.columnMagnitude(2);
	ra_ = 1.0/a_;
	rb_ = 1.0/b_;
	rc_ = 1.0/c_;
}

/*!
 * \brief Destructor
 * \details Constructor for OrthorhombicBox.
 */
OrthorhombicBox::~OrthorhombicBox()
{
}

/*
// Minimum Image Routines (virtual implementations)
*/

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> OrthorhombicBox::minimumImage(const Atom* i, const Atom* ref) const
{
	Vec3<double> mimVec = i->r();
	mimVec -= ref->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec + ref->r();
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> OrthorhombicBox::minimumImage(const Atom* i, const Vec3<double>& ref) const
{
	Vec3<double> mimVec = i->r();
	mimVec -= ref;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec + ref;
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'ref'
 */
Vec3<double> OrthorhombicBox::minimumImage(const Vec3<double>& i, const Vec3<double>& ref) const
{
	Vec3<double> mimVec = i;
	mimVec -= ref;	
	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec + ref;
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> OrthorhombicBox::minimumVector(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec;
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> OrthorhombicBox::minimumVector(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec;
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> OrthorhombicBox::minimumVector(const Vec3<double>& i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec;
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistance(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistance(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistance(const Vec3<double>& i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitude();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistanceSquared(const Atom* i, const Atom* j) const
{
	Vec3<double> mimVec = j->r();
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistanceSquared(const Atom& i, const Atom& j) const
{
	Vec3<double> mimVec = j.r();
	mimVec -= i.r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistanceSquared(const Atom* i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i->r();

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double OrthorhombicBox::minimumDistanceSquared(const Vec3<double>& i, const Vec3<double>& j) const
{
	Vec3<double> mimVec = j;
	mimVec -= i;

	mimVec.x -= int(mimVec.x*ra_ + (mimVec.x < 0.0 ? -0.5 : 0.5))*a_;
	mimVec.y -= int(mimVec.y*rb_ + (mimVec.y < 0.0 ? -0.5 : 0.5))*b_;
	mimVec.z -= int(mimVec.z*rc_ + (mimVec.z < 0.0 ? -0.5 : 0.5))*c_;

	return mimVec.magnitudeSq();
}

/*
// Utility Routines (Virtual Implementations)
*/

/*!
 * \brief Return random coordinate inside Box
 */
Vec3<double> OrthorhombicBox::randomCoordinate() const
{
	Vec3<double> pos;
	pos.x = a_*DUQMath::random();
	pos.y = b_*DUQMath::random();
	pos.z = c_*DUQMath::random();
	return pos;
}

/*!
 * \brief Return folded coordinate (i.e. inside current Box)
 */
Vec3<double> OrthorhombicBox::fold(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	Vec3<double> frac(r.x*ra_, r.y*rb_, r.z*rc_);
	
	// Fold into Box and divide by integer Cell sizes
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);
	frac.x *= a_;
	frac.y *= b_;
	frac.z *= c_;
	return frac;
}

/*!
 * \brief Return folded fractional coordinate (i.e. inside current Box)
 */
Vec3<double> OrthorhombicBox::foldFrac(const Vec3<double>& r) const
{
	// Convert coordinate to fractional coords
	Vec3<double> frac(r.x*ra_, r.y*rb_, r.z*rc_);
	
	// Fold into Box and divide by integer Cell sizes
	frac.x -= floor(frac.x);
	frac.y -= floor(frac.y);
	frac.z -= floor(frac.z);
	return frac;
}

/*!
 * \brief Convert supplied fractional coordinates to real space
 */
Vec3<double> OrthorhombicBox::fracToReal(const Vec3<double>& r) const
{
	// Multiply by box lengths
	return Vec3<double>(r.x*a_, r.y*b_, r.z*c_);
}