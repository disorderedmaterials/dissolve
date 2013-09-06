/*
	*** Monoclinic Box
	*** src/lib/classes/box_monoclinic.cpp
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

/*!
 * \brief Constructor
 * \details Constructor for Monoclinic Box. 
 */
MonoclinicBox::MonoclinicBox(double volume, const Vec3<double> relativeCellLengths, double alpha) : Box()
{
	type_ = Box::MonoclinicBox;
	
	// Construct axes_
	alpha_ = alpha;
	// Assume that A lays along x-axis, and B lays along the y-axis (since gamma = 90)
	axes_.setColumn(0, 1.0, 0.0, 0.0);
	axes_.setColumn(1, 0.0, 1.0, 0.0);
	// The C vector will only have components in y and z (since beta = 90)
	double cosAlpha = cos(alpha_/DEGRAD);
	axes_.setColumn(2, 0.0, cosAlpha, sqrt(1.0-cosAlpha*cosAlpha));

	// Rescale to desired volume
	setVolume(volume);

	// Grab new cell lengths
	a_ = axes_.columnMagnitude(0);
	b_ = axes_.columnMagnitude(1);
	c_ = axes_.columnMagnitude(2);
	
	// Calculate inverse
	inverseAxes_ = axes_;
	inverseAxes_.invert();
}

/*!
 * \brief Destructor
 * \details Constructor for MonoclinicBox.
 */
MonoclinicBox::~MonoclinicBox()
{
}

/*
// Minimum Image Routines (virtual implementations)
*/

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'j'
 */
Vec3<double> MonoclinicBox::minimumImage(const Atom* i, const Atom* ref) const
{
	msg.print("MonoclinicBox::minimumImage() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'j'
 */
Vec3<double> MonoclinicBox::minimumImage(const Atom* i, const Vec3<double>& ref) const
{
	msg.print("MonoclinicBox::minimumImage() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image coordinates of 'i' with respect to 'j'
 */
Vec3<double> MonoclinicBox::minimumImage(const Vec3<double>& i, const Vec3<double>& ref) const
{
	msg.print("MonoclinicBox::minimumImage() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> MonoclinicBox::minimumVector(const Atom* i, const Atom* j) const
{
	msg.print("MonoclinicBox::minimumVector() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> MonoclinicBox::minimumVector(const Atom* i, const Vec3<double>& j) const
{
	msg.print("MonoclinicBox::minimumVector() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image vector from 'i' to 'j'
 */
Vec3<double> MonoclinicBox::minimumVector(const Vec3<double>& i, const Vec3<double>& j) const
{
	msg.print("MonoclinicBox::minimumVector() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double MonoclinicBox::minimumDistance(const Atom* i, const Atom* j) const
{
	return minimumVector(i, j).magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double MonoclinicBox::minimumDistance(const Atom* i, const Vec3<double>& j) const
{
	return minimumVector(i, j).magnitude();
}

/*!
 * \brief Return minimum image distance from 'i' to 'j'
 */
double MonoclinicBox::minimumDistance(const Vec3<double>& i, const Vec3<double>& j) const
{
	return minimumVector(i, j).magnitude();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j' (pointers)
 */
double MonoclinicBox::minimumDistanceSquared(const Atom* i, const Atom* j) const
{
	return minimumVector(i, j).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j' (references)
 */
double MonoclinicBox::minimumDistanceSquared(const Atom& i, const Atom& j) const
{
	return minimumVector(i.r(), j.r()).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double MonoclinicBox::minimumDistanceSquared(const Atom* i, const Vec3<double>& j) const
{
	return minimumVector(i, j).magnitudeSq();
}

/*!
 * \brief Return minimum image squared distance from 'i' to 'j'
 */
double MonoclinicBox::minimumDistanceSquared(const Vec3<double>& i, const Vec3<double>& j) const
{
	return minimumVector(i, j).magnitudeSq();
}

/*
// Utility Routines (Virtual Implementations)
*/

/*!
 * \brief Return random coordinate inside Box
 */
Vec3<double> MonoclinicBox::randomCoordinate() const
{
	msg.print("MonoclinicBox::randomCoordinate() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return folded coordinate (i.e. inside current Box)
 */
Vec3<double> MonoclinicBox::fold(const Vec3<double>& r) const
{
// 	// Convert coordinate to fractional coords
// 	Vec3<double> frac(r.x/a_, r.y/a_, r.z/a_);
// 	
// 	// Fold into Box and divide by integer Cell sizes
// 	frac.x -= floor(frac.x);
// 	frac.y -= floor(frac.y);
// 	frac.z -= floor(frac.z);
// 	return frac*a_;
	msg.print("MonoclinicBox::fold() not written yet.\n");
	return Vec3<double>();
}

/*!
 * \brief Return folded fractional coordinate (i.e. inside current Box)
 */
Vec3<double> MonoclinicBox::foldFrac(const Vec3<double>& r) const
{
// 	// Convert coordinate to fractional coords
// 	Vec3<double> frac(r.x/a_, r.y/a_, r.z/a_);
// 	
// 	// Fold into Box and divide by integer Cell sizes
// 	frac.x -= floor(frac.x);
// 	frac.y -= floor(frac.y);
// 	frac.z -= floor(frac.z);
// 	return frac*a_;
	msg.print("MonoclinicBox::foldFrac() not written yet.\n");
	return Vec3<double>();
}
