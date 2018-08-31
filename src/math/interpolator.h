/*
	*** Interpolator
	*** src/math/interpolator.h
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

#ifndef DISSOLVE_INTERPOLATOR_H
#define DISSOLVE_INTERPOLATOR_H

#include "templates/array.h"

// Forward Declarations
class Data1D;

// Interpolator
class Interpolator
{
	public:
	// Interpolation Schemes
	enum InterpolationScheme
	{
		NoInterpolation,
		SplineInterpolation,
		/* ConstrainedSplineInterpolation, */  // Removed for now as it produces spurious features in some fits.
		LinearInterpolation,
		ThreePointInterpolation
	};
	// Constructors
	Interpolator(const Array<double>& x, const Array<double>& y, InterpolationScheme scheme = SplineInterpolation);
	Interpolator(const Data1D& source, InterpolationScheme scheme = SplineInterpolation);
	// Destructor
	~Interpolator();


	/*
	 * Interpolation
	 */
	private:
	// Target x array
	const Array<double>& x_;
	// Target y array
	const Array<double>& y_;
	// Interpolation scheme currently employed
	InterpolationScheme scheme_;
	// Interpolation parameters
	Array<double> a_, b_, c_, d_, h_;
	// Interval of last returned interpolated point
	int lastInterval_;

	private:
	// Prepare natural spline interpolation of data
	void interpolateSpline();
	// Prepare constrained natural spline interpolation of data
	void interpolateConstrainedSpline();
	// Prepare linear interpolation of data
	void interpolateLinear();
	// Prepare three-point interpolation of data
	void interpolateThreePoint();

	public:
	// Regenerate using specified scheme
	void interpolate(InterpolationScheme scheme = SplineInterpolation);
	// Return interpolated y value for supplied x
	double y(double x);
	// Return interpolated y value for supplied x, specifying containing interval
	double y(double x, int interval);


	/*
	 * Static Functions
	 */
	public:
	// Approximate y at specified x value using three-point interpolation of supplied data
	static double approximate(const Data1D& data, double x);
	// Add interpolated data B to data A, with supplied multiplication factor
	static void addInterpolated(Data1D& A, const Data1D& B, double factor = 1.0);
};

#endif
