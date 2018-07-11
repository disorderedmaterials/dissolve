/*
	*** XY Data - Interpolation
	*** src/base/xydata_interp.cpp
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

#include "math/xydata.h"

// Construct natural spline interpolation of data
void XYData::interpolateSpline()
{
	/* For a set of N points {x(i),y(i)} for i = 0 - (N-1), the i'th interval can be represented by a cubic spline
	 * 
	 * 	S(i)(x) = a(i) + b(i)(x - x(i)) + c(i)(x - x(i))**2 + d(i)h(x - x(i))**3.
	 * 
	 * As such, there are 4N unknown coefficients - {a(0),b(0),c(0),d(0),a(1),b(1),c(1),d(1),...,a(N-1),b(N-1),c(N-1),d(N-1)}
	 * 
	 * Conditions:
	 * 	At the extreme values of x in each interval, the original y values are retained
	 * 		S(i)(x = x(i)) = y(i)    and 	S(i)(x = x(i+1)) = y(i+1)		== 2N equations
	 * 	First derivatives are continuous across intervals (i.e. match at extreme x values of intervals)
	 * 		S'(i)(x = x(i+1)) = S'(i+1)(x = x(i+1))					== (N-1) equations
	 * 	Second derivatives are continuous across intervals (i.e. match at extreme x values of intervals)
	 * 		S"(i)(x = x(i+1)) = S"(i+1)(x = x(i+1))					== (N-1) equations
	 * 
	 * So, there are 4N-2 equations specified here - the remaining two come from specifying endpoint conditions of the spline fit. 
	 * No derivative matching is (can) be performed at the extreme points i=0 and i=N-1, so specify exact derivatives at these points:
	 * 	Natural splines:
	 * 		S'(0) = 0	S'(N-1) = 0
	 * 	Clamped splines
	 *
	 * We need expressions for the derivatives of S:
	 * 	Let h(i) = x - x(i)
	 * 	S(i)(x) = a(i) + b(i)h(i) + c(i)h(i)**2 + d(i)h(i)**3
	 * 	S'(i)(x) = b(i) + 2 c(i)h(i) + 3 d(i)h(i)**2
	 * 	S"(i)(x) = 2 c(i) + 6 d(i)h(i)
	 * 
	 * From the conditions outlined above:
	 * 	S(i)(x = x(i)) = y(i)	  ==>	a(i) = y(i)	since x = x(i) and so h(i) = x(i) - x(i) = 0
	 * 	S(i)(x = x(i+1)) = y(i+1) ==>	a(i) + b(i)h(i) + c(i)h(i)**2 + d(i)h(i)**3 = y(i+1)
	 * 	S'(i)(x = x(i+1)) = S'(i+1)(x = x(i+1))   ==>	b(i) + 2 c(i)h(i) + 3 d(i)h(i)**2 - b(i+1) = 0
	 * 							since at x = x(i+1), h(i+1) = 0 and so S'(i+1)(x = x(i+1)) = b(i+1)
	 * 	S"(i)(x = x(i+1)) = S"(i+1)(x = x(i+1))   ==>	2 c(i) + 6 d(i)h(i) - 2 c(i+1) = 0
	 * 							since at x = x(i+1), h(i+1) = 0 and so S"(i+1)(x = x(i+1)) = 2 c(i+1)
	 * To summarise:
	 * 	a(i) = y(i)						for i = 0,N-1	(N eqs)
	 * 	a(i) + b(i)h(i) + c(i)h(i)**2 + d(i)h(i)**3 = y(i+1)	for i = 0,N-1	(N eqs)
	 * 	b(i) + 2 c(i)h(i) + 3 d(i)h(i)**2 - b(i+1) = 0		for i = 0,N-2	(N-1 eqs)
	 * 	2 c(i) + 6 d(i)h(i) - 2 c(i+1) = 0			for i = 0,N-2	(N-1 eqs)
	 * 
	 * To simplify, consider value of second derivative in i'th interval at leftmost value:
	 * 	S"(i)(x = x(i)) = 2 c(i)	since at x = x(i+1), h(i+1) = 0 and so S"(i+1)(x = x(i+1)) = 2 c(i+1)
	 * We will call these coefficients m(i):
	 * 	2 c(i) + 6 d(i)h(i) - 2 c(i+1) = 0	==>	m(i) + 6 d(i)h(i) - m(i+1) = 0
	 * 						==>	d(i) = m(i+1) - m(i)
	 * 							       -------------
	 * 								   6 h(i)
	 * 											  m(i)		m(i+1) - m(i)
	 * 	y(i) + b(i)h(i) + c(i)h(i)**2 + d(i)h(i)**3 = y(i+1)	==>	y(i) + b(i)h(i) + --- h(i)**2 + ------------- h(i)**3 = y(i+1)
	 * 											   2		    6 h(i)
	 * 
	 * 									       y(i+1) - y(i)   m(i)	  m(i+1) - m(i)
	 * 								==>	b(i) = ------------- - --- h(i) - ------------- h(i)
	 * 										    h(i)	2	        6
	 * 
	 * 
	 * 	b(i) + 2 c(i)h(i) + 3 d(i)h(i)**2 = b(i+1)	==>	
	 * 	( y(i+1) - y(i)   m(i)	     m(i+1) - m(i)	)		 m(i+1) - m(i)		 ( y(i+2) - y(i+1)   m(i+1)	     m(i+2) - m(i+1)        )
	 * 	( ------------- - --- h(i) - ------------- h(i) ) + m(i)h(i) + 3 ------------- h(i)**2 = ( --------------- - ------ h(i+1) - --------------- h(i+1) )
	 * 	(      h(i)	   2	           6		)		     6 h(i)		 (      h(i+1)	        2	            6	            ) 
	 * 
	 * 								      ( y(i+2) - y(i+1)   y(i+1) - y(i) )
	 * 	==>	h(i)m(i) + 2 (h(i) + h(i+1))m(i+1) + h(i+1)m(i+2) = 6 ( --------------- - ------------- )
	 * 								      (      h(i+1)           h(i)	)
	 * 
	 * Written in matrix form, the above system of equations is:
	 * 
	 * 	[ 1		0		0		0	...	0 ] [  m(0)  ]     [			0			 ]
	 * 	[ h(0)		2(h(0) + h(1))	h(1)		0	...	0 ] [  m(1)  ]     [ (y(2) - y(1)) / h(1) - (y(1) - y(0)) / h(0) ]
	 * 	[ 0		h(1)		2(h(1) + h(2))	h(2)	...	0 ] [  m(2)  ] = 6 [ (y(3) - y(2)) / h(2) - (y(2) - y(1)) / h(1) ]
	 * 	[			...				...	  ] [  ...   ]     [						 ]
	 * 	[ 0		0		0		0	...	1 ] [ m(N-1) ]     [ 			0			 ]
	 * 
	 * Once solved, we have the set of m() and can thus determine a, b, c, and d as follows:
	 *
	 *			       y(i+1)-y(i)   h(i)        h(i)			       m(i)	       m(i+1)-m(i)
	 * 	a(i) = y(i)	b(i) = ----------- - ---- m(i) - ---- (m(i+1)-m(i))	c(i) = ----	d(i) = -----------
	 * 				  h(i)	      2	          6				2		  6 h(i)
	 * 
	 * The constrained spline fit removes the requirement that the second derivatives are equal at the boundaries of every interval, and instead specifies
	 * that an exact first derivative is obtained there instead. As such:
	 * 
	 * 	S"(i)(x = x(i+1)) = S"(i+1)(x = x(i+1))	  ==>	S'(i)(x = x(i+1)) = S'(i+1)(x = x(i+1)) = f'(x = x(i+1))
	 * 
	 * A suitable estimate of the slope comes from consideration of the slopes of the two adjacent intervals to x(i):
	 * 
	 * 		      ( x(i+1)-x(i)   x(i)-x(i-1) )
	 * 	f'(i) = 0.5 * ( ----------- + ----------- )	for i=1, N-2
	 * 		      ( y(i+1)-y(i)   y(i)-y(i-1) )
	 * 
	 * At the endpoints:
	 * 
	 * 		3( y(1)-y(0) )   f'(1)			  3( y(i-1) - y(n-2) )   f'(N-1)
	 * 	f'(0) = -------------- - -----		f'(N-1) = -------------------- - -------
	 * 		2( x(1)-x(0) )     2			  2( x(i-1) - x(n-2) )      2
	 * 
	 * Since the slope at each point is known, there is no longer a need to solve a system of equations, and each coefficient may be calculated as follows:
	 * 
	 * 		  2( f'(i)+2f'(i-1) )   6(y(i) - y(i-1)
	 * 	S"(i) =   ------------------- - ----------------	and  S"(i-1) = -S"(i)
	 * 		     x(i) - x(i-1)	(x(i)-x(i-1))**2
	 * 
	 * 	       S"(i) - S"(i-1)		       x(i)S"(i-1)-x(i-1)S"(i)
	 * 	d(i) = ---------------		c(i) = -----------------------
	 * 	       6(x(i) - x(i-1))			   2(x(i) - x(i-1))
	 * 
	 * 	       (y(i) - y(i-1)) - c(i)(x(i)**2 - x(i-1)**2) - d(i)(x(i)**3 - x(i-1)**3)
	 * 	b(i) =------------------------------------------------------------------------
	 * 					x(i) - x(i-1)
	 */
	
	int i, nPoints = x_.nItems();

	if (nPoints < 2) return;
	
	// Calculate interval array 'h'
	interpolationH_.initialise(nPoints);
	for (i=0; i<nPoints-1; ++i) interpolationH_[i] = x_[i+1] - x_[i];

	// Initialise parameter arrays and working array
	interpolationA_.initialise(nPoints-1);
	interpolationB_.initialise(nPoints-1);
	interpolationC_.initialise(nPoints);
	interpolationD_.initialise(nPoints-1);

	// Determine 'm' values (== C) now by solving the tridiagonal matrix above. Use Thomas algorithm...
	// -- First stage
	double p, q, r, s;
	Array<double> rprime(nPoints), sprime(nPoints);
	rprime[0] = 0.0;	// Would be 0.0 / 1.0 in the case of Natural spline
	sprime[0] = 0.0;

	for (i=1; i<nPoints-1; ++i)
	{
		// For a given i, p(i) = h(i-1), q(i) = 2(h(i-1)+h(i)), r(i) = h(i), s(i) = 6 ((y(n+1) - y(n)) / h(n) - (y(n) - y(n-1)) / h(n-1)
		p = interpolationH_[i-1];
		q = 2.0*(interpolationH_[i-1]+interpolationH_[i]);
		r = interpolationH_[i];
		s = 6.0 * ((y_[i+1] - y_[i]) / interpolationH_[i] - (y_[i] - y_[i-1]) / interpolationH_[i-1]);

		// -- Calculate r'(i) = r(i) / ( q(i) - r'(i-1)p(i) )
		rprime[i] = r / (q - rprime[i-1]*p);
		// -- Calculate s'(i) = (s(i) - s'(i-1)p(i) ) / ( q(i) - r'(i-1)p(i))
		sprime[i] = (s - sprime[i-1]*p) / (q - rprime[i-1]*p);
	}
	rprime[nPoints-1] = 0.0;
	sprime[nPoints-1] = (0.0 - sprime[nPoints-2]/interpolationH_[i-1]) / (2.0*interpolationH_[i-1]);

	// -- Second stage - backsubstitution
	interpolationC_[nPoints-1] = 0.0;
	for (i=nPoints-2; i>=0; --i)
	{
		// For a given i, m(i) = s'(i) - r'(i)m(i+1)
		interpolationC_[i] = sprime[i] - rprime[i]*interpolationC_[i+1];
	}
	
	// interpolationC_ array now contains m(i)...
	for (i=0; i<nPoints-1; ++i)
	{
		interpolationB_[i] = (y_[i+1] - y_[i]) / interpolationH_[i] - 0.5 * interpolationH_[i] * interpolationC_[i] - (interpolationH_[i] * (interpolationC_[i+1]-interpolationC_[i]))/6.0;
		interpolationD_[i] = (interpolationC_[i+1] - interpolationC_[i]) / (6.0 * interpolationH_[i]);
		interpolationC_[i] *= 0.5;
		interpolationA_[i] = y_[i];
	}

	interpolationInterval_ = 0;
}

// Prepare constrained natural spline interpolation of data
void XYData::interpolateConstrainedSpline()
{
	/*
	 * Constrained Spline fit
	 */

	int i, nPoints = x_.nItems();

	// Initialise parameter arrays and working array
	interpolationA_.initialise(nPoints-1);
	interpolationB_.initialise(nPoints-1);
	interpolationC_.initialise(nPoints-1);
	interpolationD_.initialise(nPoints-1);

	// Calculate first derivatives at each point
	Array<double> fp(nPoints);
	double gradA, gradB;
	for (i=1; i<nPoints-1; ++i)
	{
		gradA = (x_[i+1] - x_[i])/(y_[i+1] - y_[i]);
		gradB = (x_[i] - x_[i-1])/(y_[i] - y_[i-1]);
		if (DissolveMath::sgn(gradA) != DissolveMath::sgn(gradB)) fp[i] = 0.0;
		else fp[i] = 2.0 / (gradA + gradB);
		
	}
// 		fp[0] = (3.0*(y_[1] - y_[0])) / (2.0*x_[1]-x_[0]) - 0.5*fp[1];
// 		fp[nPoints-1] = (3.0*(y_[nPoints-1] - y_[nPoints-2])) / (2.0*x_[nPoints-1]-x_[nPoints-2]) - 0.5*fp[nPoints-2];
	fp[0] = 0.0;
	fp[nPoints-1] = 0.0;

	// Calculate coefficients
	double fppi, fppim1, dx, dy;
	for (i=1; i<nPoints; ++i)
	{
		dx = x_[i] - x_[i-1];
		dy = y_[i] - y_[i-1];
		fppim1 = -2.0*(fp[i]+2.0*fp[i-1]) / dx + 6.0*dy/(dx*dx);
		fppi = 2.0*(2.0*fp[i]+fp[i-1]) / dx - 6.0*dy/(dx*dx);
		interpolationD_[i-1] = (fppi - fppim1) / (6.0*dx);
		interpolationC_[i-1] = (x_[i]*fppim1 - x_[i-1]*fppi) / (2.0*dx);
		interpolationB_[i-1] = (dy - interpolationC_[i-1]*(x_[i]*x_[i] - x_[i-1]*x_[i-1]) - interpolationD_[i-1]*(x_[i]*x_[i]*x_[i] - x_[i-1]*x_[i-1]*x_[i-1])) / dx;
		interpolationA_[i-1] = y_[i-1] - interpolationB_[i-1]*x_[i-1] - interpolationC_[i-1]*x_[i-1]*x_[i-1] - interpolationD_[i-1]*x_[i-1]*x_[i-1]*x_[i-1];
	}

	interpolationInterval_ = 0;
}

// Prepare linear interpolation of data
void XYData::interpolateLinear()
{
	// Calculate y interval array 'a'
	interpolationA_.initialise(x_.nItems()-1);
	for (int i=0; i<x_.nItems()-1; ++i) interpolationA_[i] = y_[i+1] - y_[i];

	interpolationInterval_ = 0;
}

// Prepare linear interpolation of data
void XYData::interpolateThreePoint()
{
	interpolationInterval_ = 0;
}

// Calculate natural spline interpolation of current data
void XYData::interpolate(XYData::InterpolationScheme scheme)
{
	if (scheme == XYData::NoInterpolation)
	{
		clearInterpolationArrays();
		interpolationScheme_ = NoInterpolation;
		return;
	}

	// Calculate interval array 'h'
	interpolationH_.initialise(x_.nItems()-1);
	for (int i=0; i<x_.nItems()-1; ++i) interpolationH_[i] = x_[i+1] - x_[i];

	if (scheme == XYData::SplineInterpolation) interpolateSpline();
// 	else if (scheme == XYData::ConstrainedSplineInterpolation) interpolateConstrainedSpline();
	else if (scheme == XYData::LinearInterpolation) interpolateLinear();
	else if (scheme == XYData::ThreePointInterpolation) interpolateThreePoint();

	interpolationScheme_ = scheme;
}

// Return spline interpolated y value for supplied x
double XYData::interpolated(double xValue)
{
	// Do we need to (re)generate the interpolation?
	if (interpolationInterval_ == -1)
	{
		// Do we know what the interpolation scheme is?
		if (interpolationScheme_ != XYData::NoInterpolation) interpolate(interpolationScheme_);
		else
		{
			// No existing interpolation scheme, so use Spline by default
			interpolate(XYData::SplineInterpolation);
		}
	}

	// Perform binary chop search
	interpolationInterval_ = 0;
	int i, right = interpolationH_.nItems()-1;
	while ((right-interpolationInterval_) > 1)
	{
		i = (right+interpolationInterval_) / 2;
		if (x_[i] > xValue) right = i;
		else interpolationInterval_ = i;
	}

	return interpolated(xValue, interpolationInterval_);
}

// Return spline interpolated y value for supplied x, specifying containing interval
double XYData::interpolated(double xValue, int interval)
{
	if (interval < 0) return y_.first();

	if (interpolationScheme_ == XYData::SplineInterpolation)
	{
		if (xValue >= x_.last()) return y_.last();

		double h = xValue - x_[interval];
		double hh = h*h;
		return interpolationA_[interval] + interpolationB_[interval]*h + interpolationC_[interval]*hh + interpolationD_[interval]*hh*h;
	}
//	else if (interpolationScheme_ == XYData::ConstrainedSplineInterpolation)
//	{
//		double h = xValue;
//		double hh = h*h;
//		return interpolationA_[interval] + interpolationB_[interval]*h + interpolationC_[interval]*hh + interpolationD_[interval]*hh*h;
//	}
	else if (interpolationScheme_ == XYData::LinearInterpolation)
	{
		if (interval >= (x_.nItems()-1)) return y_.last();

		double delta = (xValue - x_.value(interval)) / interpolationH_.value(interval);
		return y_.value(interval) + delta * interpolationA_.value(interval);
	}
	else if (interpolationScheme_ == XYData::ThreePointInterpolation)
	{
		if (interval >= (x_.nItems()-3)) return y_.last();

		double ppp = (xValue - x_.value(interval)) / interpolationH_.value(interval);

		double vk0 = y_.value(interval);
		double vk1 = y_.value(interval+1);
		double vk2 = y_.value(interval+2);
		double t1=vk0+(vk1-vk0)*ppp;
		double t2=vk1+(vk2-vk1)*(ppp-1.0);
// 		printf("%f %20.14e %20.14e %20.14e %20.14e %20.14e\n", xValue, vk0, vk1, vk2, ppp, t1+(t2-t1)*ppp*0.5); 
		return t1+(t2-t1)*ppp*0.5;
	}

	return 0.0;
}

// Approximate data at specified x value using three-point interpolation
double XYData::approximate(double xValue) const
{
	if (xValue < x_.first()) return y_.first();
	if (xValue > x_.last()) return y_.last();

	// Perform binary chop search
	int left = 0;
	int i, right = x_.nItems() - 1;
	while ((right-left) > 1)
	{
		i = (right+left) / 2;
		if (x_.value(i) > xValue) right = i;
		else left = i;
	}
// 	printf("L/R = %i/%i : %f < %f < %f\n", left, right, x_.value(left), xValue, x_.value(right));

	double ppp = (xValue - x_.value(left)) / (x_.value(right) - x_.value(left));

	double vk0 = y_.value(left);
	double vk1 = y_.value(left+1);
	double vk2 = y_.value(left+2);

	double t1=vk0+(vk1-vk0)*ppp;
	double t2=vk1+(vk2-vk1)*(ppp-1.0);
// 		printf("%f %20.14e %20.14e %20.14e %20.14e %20.14e\n", xValue, vk0, vk1, vk2, ppp, t1+(t2-t1)*ppp*0.5); 
	return t1+(t2-t1)*ppp*0.5;
}

// Expand the current data, adding n interpolated points in-between the original values
void XYData::expand(int nExtraPoints)
{
	Array<double> newX, newY;

	int nDivisions = nExtraPoints + 1;
	double range, delta, x;

	for (int n=0; n<x_.nItems()-1; ++n)
	{
		range = x_[n+1] - x_[n];
		delta = range / nDivisions;

		// Add points
		x = x_[n];
		for (int m=0; m<nDivisions; ++m)
		{
			newX.add(x);
			newY.add(interpolated(x));
			x += delta;
		}
	}

	// Copy new data over existing
	x_ = newX;
	y_ = newY;
	interpolationInterval_ = 0;
}
