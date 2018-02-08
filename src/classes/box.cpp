/*
	*** Box Definition
	*** src/classes/box.cpp
	Copyright T. Youngs 2012-2018

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
#include "classes/cell.h"
#include "base/processpool.h"
#include "base/xydata.h"
#include <string.h>

// Constructor
Box::Box()
{
	type_ = Box::nBoxTypes;
	periodic_.set(true, true, true);
	volume_ = 0.0;
}

// Virtual Destructor
Box::~Box()
{
}

// Box Type Keywords
const char* BoxTypeKeywords[] = { "Non-Periodic", "Cubic", "Orthorhombic", "Monoclinic", "Triclinic" };

// Convert text string to BoxType
Box::BoxType Box::boxType(const char* s)
{
	for (int n=0; n<Box::nBoxTypes; ++n) if (strcmp(s,BoxTypeKeywords[n]) == 0) return (Box::BoxType) n;
	return Box::nBoxTypes;
}

// Convert BoxType to text string
const char* Box::boxType(Box::BoxType id)
{
	return BoxTypeKeywords[id];
}

// Assignment operator
void Box::operator=(const Box& source)
{
	// Basic Definition
	type_ = source.type_;
	a_ = source.a_;
	b_ = source.b_;
	c_ = source.c_;
	ra_ = source.ra_;
	rb_ = source.rb_;
	rc_ = source.rc_;
	alpha_ = source.alpha_;
	beta_ = source.beta_;
	gamma_ = source.gamma_;
	axes_ = source.axes_;
	inverseAxes_ = source.inverseAxes_;
	periodic_ = source.periodic_;
}

/*
 * Basic Definition
 */

// Return Box type
Box::BoxType Box::type() const
{
	return type_;
}

// Set up box, scaling to specified volume (in cubic Angstroms)
void Box::setUp(double volume)
{
	if (volume > 0.0)
	{
		Messenger::print("--> Current box volume is %f cubic Angstroms, requested = %f\n", axes_.determinant(), volume);
		double factor = pow(volume,1.0/3.0) / pow(axes_.determinant(),1.0/3.0);
		Messenger::print("--> Scaling factor = %f\n", factor);
		axes_.applyScaling(factor, factor, factor);
	}
	else Messenger::print("--> Current box volume is %f cubic Angstroms and will not be altered.\n", axes_.determinant());

	// Calculate box volume
	volume_ = axes_.determinant();

	// Calculate inverse axes
	inverseAxes_ = axes_;
	inverseAxes_.invert();

	// Calculate reciprocal axes and volume
	// Reciprocal cell vectors are perpendicular to normal cell axes.
	// Calculate from cross products of normal cell vectors
	reciprocalAxes_.setColumn(0, axes_.columnAsVec3(1) * axes_.columnAsVec3(2));
	reciprocalAxes_.setColumn(1, axes_.columnAsVec3(2) * axes_.columnAsVec3(0));
	reciprocalAxes_.setColumn(2, axes_.columnAsVec3(0) * axes_.columnAsVec3(1));
	reciprocalAxes_.columnMultiply(0, TWOPI / volume_);
	reciprocalAxes_.columnMultiply(1, TWOPI / volume_);
	reciprocalAxes_.columnMultiply(2, TWOPI / volume_);
	reciprocalVolume_ = (reciprocalAxes_.columnAsVec3(1) * reciprocalAxes_.columnAsVec3(2)).dp(reciprocalAxes_.columnAsVec3(0));

	Messenger::print("--> Final box volume is %f cubic Angstroms (reciprocal volume = %e)\n", volume_, reciprocalVolume_);
}

// Return volume
double Box::volume() const
{
	return volume_;
}

// Return axis length specified
double Box::axisLength(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n > 2))
	{
		Messenger::print("OUT_OF_RANGE - Requested length for a bad axis (%i) in Box::axisLength().\n", n);
		return 0.0;
	}
#endif
	return axes_.columnMagnitude(n);
}

// Return axis angle specified
double Box::axisAngle(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n > 2))
	{
		Messenger::print("OUT_OF_RANGE - Requested angle for a bad axis (%i) in Box::axisAngle().\n", n);
		return 0.0;
	}
#endif
	Vec3<double> u, v;
	u = axes_.columnAsVec3((n+1)%3);
	v = axes_.columnAsVec3((n+2)%3);
	u.normalise();
	v.normalise();
	double dp = u.dp(v);
	return acos(dp)*DEGRAD;
}

// Return axes matrix
const Matrix3& Box::axes() const
{
	return axes_;
}

// Return inverse axes matrix
const Matrix3& Box::inverseAxes() const
{
	return inverseAxes_;
}

// Return reciprocal box volume
double Box::reciprocalVolume() const
{
	return reciprocalVolume_;
}

// Return reciprocal axis lengths
Vec3<double> Box::reciprocalAxisLengths() const
{
	return Vec3<double>(reciprocalAxes_.columnMagnitude(0), reciprocalAxes_.columnMagnitude(1), reciprocalAxes_.columnMagnitude(2));
}

// Return reciprocal axes matrix
const Matrix3& Box::reciprocalAxes() const
{
	return reciprocalAxes_;
}

/*
 * Utility Routines
 */

// Return radius of largest possible inscribed sphere for box
double Box::inscribedSphereRadius() const
{
	// Radius of largest inscribed sphere is the smallest of the three calculated values....
	double mag, diameter, result = 0.0;
	Vec3<double> cross;
	int n, i, j;
	for (n=0; n<3; ++n)
	{
		i = (n+1)%3;
		j = (n+2)%3;
		cross = axes_.columnAsVec3(i) * axes_.columnAsVec3(j);
		mag = cross.magnitude();
		diameter = fabs(axes_.columnAsVec3(n).dp(cross / mag));
		if (n == 0) result = diameter;
		else if (diameter < result) result = diameter;
	}
	return result*0.5;
}

// Calculate the RDF normalisation for the Box
bool Box::calculateRDFNormalisation(ProcessPool& procPool, XYData& boxNorm, double rdfRange, double rdfBinWidth, int nPoints) const
{
	// Set up array - we will use a nominal bin width of 0.1 Angstroms and then interpolate to the rdfBinWidth afterwards
	const double binWidth = 0.1;
	const double rBinWidth = 1.0/binWidth;
	int bin, nBins = rdfRange / binWidth;
	XYData normData;
	normData.initialise(nBins);
	Array<double>& y = normData.arrayY();
	for (int n=0; n<nBins; ++n) normData.arrayX()[n] = (n+0.5)*binWidth;
	
	Vec3<double> centre = axes_*Vec3<double>(0.5,0.5,0.5);

	// Divide points over processes
	const int nPointsPerProcess = nPoints / procPool.nProcesses();
	Messenger::print("--> Number of insertion points per process is %i, total is %i\n", nPointsPerProcess, nPointsPerProcess*procPool.nProcesses());

	// Pre-waste random numbers so that the random number generators in all processes line up
	for (int n=0; n<nPointsPerProcess*procPool.poolRank(); ++n) randomCoordinate();

	// Calculate the function
	y = 0.0;
	for (int n=0; n<nPointsPerProcess; ++n)
	{
		bin = (randomCoordinate() - centre).magnitude() * rBinWidth;
		if (bin < nBins) y[bin] += 1.0;
	}
	if (!procPool.allSum(y.array(), nBins)) return false;

	// Post-waste random numbers so that the random number generators in all processes line up
	for (int n=0; n<nPointsPerProcess*(procPool.nProcesses()-1-procPool.poolRank()); ++n) randomCoordinate();

	// Normalise histogram data, and scale by volume and binWidth ratio
	y /= double(nPointsPerProcess*procPool.nProcesses());
	y *= volume_ * (rdfBinWidth / binWidth);
	normData.interpolate(XYData::SplineInterpolation);

	// Now we have the interpolated data, create the proper interpolated data
	nBins = rdfRange/rdfBinWidth;
	boxNorm.clear();

	// Rescale against expected volume for spherical shells
	double shellVolume, r = 0.0, maxHalf = inscribedSphereRadius(), x = 0.5*rdfBinWidth;
	for (int n=0; n<nBins; ++n)
	{
		// We know that up to the maximum (orthogonal) half-cell width the normalisation should be exactly 1.0...
		if (x < maxHalf) boxNorm.addPoint(x, 1.0);
		else
		{
			shellVolume = (4.0/3.0)*PI*(pow(r+rdfBinWidth,3.0) - pow(r,3.0));
			boxNorm.addPoint(x,  shellVolume / normData.interpolated(x));
// 			boxNorm[n] = normData.interpolated(r);
		}
		r += rdfBinWidth;
		x += rdfBinWidth;
	}

	// Interpolate normalisation array
	boxNorm.interpolate(XYData::SplineInterpolation);

	return true;
}

/*
 * Utility Routines
 */

// Return angle (in degrees, no MIM) between Atoms
double Box::angleInDegrees(const Atom* i, const Atom* j, const Atom* k) const
{
	Vec3<double> vecji, vecjk;

	// Ge minimum image vectors 'j-i' and 'j-k'
	vecji = minimumVector(j, i);
	vecjk = minimumVector(j, k);
	
	// Normalise vectors
	vecji.normalise();
	vecjk.normalise();

	// Determine Angle energy
	return angleInDegrees(vecji, vecjk);

}

// Return angle (in degrees, no MIM) between coordinates
double Box::angleInDegrees(const Vec3<double>& i, const Vec3<double>& j, const Vec3<double>& k)
{
	static Vec3<double> vecji, vecjk;
	vecji = i - j;
	vecji.normalise();
	vecjk = k - j;
	vecjk.normalise();
	return acos(vecji.dp(vecjk)) * DEGRAD;
}

// Return angle (in degrees) between supplied normalised vectors
double Box::angleInDegrees(const Vec3<double>& normji, const Vec3<double>& normjk)
{
	return acos(normji.dp(normjk)) * DEGRAD;
}

// Return angle (in degrees) between supplied normalised vectors
double Box::angleInDegrees(const Vec3<double>& normji, const Vec3<double>& normjk, double& dotProduct)
{
	dotProduct = normji.dp(normjk);
	return acos(dotProduct) * DEGRAD;
}

// Return torsion (in degrees, no MIM) between supplied unnormalised vectors
double Box::torsionInDegrees(const Vec3<double>& vecji, const Vec3<double>& vecjk, const Vec3<double>& veckl)
{
	// Calculate cross products and torsion angle formed (in radians)
	Vec3<double> xpj, xpk;
	double magxpj, magxpk;

	return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk)*DEGRAD;
}

// Return torsion (in degrees, no MIM) between supplied unnormalised vectors
double Box::torsionInDegrees(const Vec3<double>& vecji, const Vec3<double>& vecjk, const Vec3<double>& veckl, Vec3<double>& xpj, double& magxpj, Vec3<double>& xpk, double& magxpk)
{
	return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk)*DEGRAD;
}

// Return torsion (in radians, no MIM) between supplied unnormalised vectors
double Box::torsionInRadians(const Vec3<double>& vecji, const Vec3<double>& vecjk, const Vec3<double>& veckl)
{
	// Calculate cross products and torsion angle formed (in radians)
	Vec3<double> xpj, xpk;
	double magxpj, magxpk;

	return torsionInRadians(vecji, vecjk, veckl, xpj, magxpj, xpk, magxpk);
}

// Return torsion (in radians, no MIM) between supplied unnormalised vectors, storing cross products and magnitude in supplied variables
double Box::torsionInRadians(const Vec3<double>& vecji, const Vec3<double>& vecjk, const Vec3<double>& veckl, Vec3<double>& xpj, double& magxpj, Vec3<double>& xpk, double& magxpk)
{
	xpj = vecjk * vecji;
	xpk = vecjk * veckl;
	magxpj = xpj.magAndNormalise();
	magxpk = xpk.magAndNormalise();
	return atan2(vecjk.dp(xpj*xpk) / vecjk.magnitude(), xpj.dp(xpk));
}

