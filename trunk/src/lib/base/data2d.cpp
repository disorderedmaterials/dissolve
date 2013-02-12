/*
	*** Two-dimensional data
	*** src/lib/base/data2d.cpp
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

#include "base/comms.h"
#include "base/data2d.h"
#include "base/messenger.h"
#include "math/constants.h"
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for Data2D.
 */
Data2D::Data2D() : ListItem<Data2D>()
{
	name_ = "Untitled";
	splineBracketLeft_ = -1;
	splineBracketRight_ = -1;
}

/*!
 * \brief Destructor
 * \details Destructor for Data2D. 
 */
Data2D::~Data2D()
{
}

/*!
 * \brief Copy Constructor
 */
Data2D::Data2D(const Data2D& source)
{
	splineBracketLeft_ = -1;
	splineBracketRight_ = -1;
	x_ = source.x_;
	y_ = source.y_;
	ddy_ = source.ddy_;
	name_ = source.name_;
}

/*!
 * \brief Clear Data
 */
void Data2D::clear()
{
	x_.clear();
	y_.clear();
	ddy_.clear();
	splineBracketLeft_ = -1;
}

/*
// Data
*/

/*!
 * \brief Resize arrays
 */
void Data2D::resize(int size)
{
	clear();
	x_.createEmpty(size);
	y_.createEmpty(size);
}

/*!
 * \brief Reset arrays to zero
 */
void Data2D::reset()
{
	for (int n=0; n<x_.nItems(); ++n) x_[n] = 0.0;
	for (int n=0; n<y_.nItems(); ++n) y_[n] = 0.0;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Initialise arrays to specified size
 */
void Data2D::initialise(int size)
{
	resize(size);
}

/*!
 * \brief Return current array size
 */
int Data2D::arraySize()
{
	return x_.nItems();
}

/*!
 * \brief Set specified data point
 */
void Data2D::setPoint(int index, double x, double y)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for x_ array in Data2D::setPoint().\n", index);
		return;
	}
	if ((index < 0) || (index >= y_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for y_ array in Data2D::setPoint().\n", index);
		return;
	}
#endif
	x_[index] = x;
	y_[index] = y;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Return number of defined datapoints
 */
int Data2D::nPoints() const
{
	return x_.nItems();
}

/*!
 * \brief Set x value
 */
void Data2D::setX(int index, double x)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for x_ array in Data2D::setX().\n", index);
		return;
	}
#endif
	x_[index] = x;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Add to x value
 */
void Data2D::addX(int index, double delta)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for x_ array in Data2D::addX().\n", index);
		return;
	}
#endif
	x_[index] += delta;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Return x value specified
 */
double Data2D::x(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for x_ array in Data2D::x().\n", index);
		return 0.0;
	}
#endif
	return x_.value(index);
}

/*!
 * \brief Return x Array
 */
Array<double>& Data2D::arrayX()
{
	splineBracketLeft_ = -1;
	return x_;
}

/*!
 * \brief Set y value
 */
void Data2D::setY(int index, double y)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for y_ array in Data2D::setY().\n", index);
		return;
	}
#endif
	y_[index] = y;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Add to y value
 */
void Data2D::addY(int index, double delta)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for y_ array in Data2D::addY().\n", index);
		return;
	}
#endif
	y_[index] += delta;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Add to y array
 */
bool Data2D::addY(const Array<double>& source, double factor)
{
	if (y_.nItems() != source.nItems())
	{
		msg.print("BAD_USAGE - Can't add Y values from source array, since the number of items differs (%i vs %i).\n", y_.nItems(), source.nItems());
		return FALSE;
	}

	for (int n=0; n<y_.nItems(); ++n) y_[n] += source.value(n)*factor;
	splineBracketLeft_ = -1;
	return TRUE;
}

/*!
 * \brief Multiply y value
 */
void Data2D::multiplyY(int index, double factor)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for y_ array in Data2D::multiplyY().\n", index);
		return;
	}
#endif
	y_[index] *= factor;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Return y value specified
 */
double Data2D::y(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Index %i is out of range for y_ array in Data2D::y().\n", index);
		return 0.0;
	}
#endif
	return y_.value(index);
}

/*!
 * \brief Return y Array
 */
Array<double>& Data2D::arrayY()
{
	splineBracketLeft_ = -1;
	return y_;
}

/*!
 * \brief Add new data point
 */
void Data2D::addPoint(double x, double y)
{
	x_.add(x);
	y_.add(y);
	splineBracketLeft_ = -1;
}

/*!
 * \brief Set name of data
 */
void Data2D::setName(const char* name)
{
	name_ = name;
}

/*!
 * \brief Return name of data
 */
const char* Data2D::name() const
{
	return name_.get();
}

/*
// Operators
*/

/*!
 * \brief Operator =
 */
void Data2D::operator=(const Data2D& source)
{
	splineBracketLeft_ = source.splineBracketLeft_;
	splineBracketRight_ = source.splineBracketRight_;
	x_ = source.x_;
	y_ = source.y_;
	ddy_ = source.ddy_;
	name_ = source.name_;
}

/*!
 * \brief Operator +
 */
Data2D Data2D::operator+(const Data2D& source) const
{
	// Construct copy of current data
	Data2D newData(*this);
	
	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		msg.error("Can't + these Data2D together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return newData;
	}

	// Add data together
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			msg.error("Can't + these Data2D, since the x axes are different.\n");
			return newData;
		}
		newData.y_[n] += source.y_.value(n);
	}
	
	newData.splineBracketLeft_ = -1;
	
	return newData;
}

/*!
 * \brief Operator +=
 */
void Data2D::operator+=(const Data2D& source)
{
	// Initialise current arrays?
	if (x_.nItems() == 0)
	{
		x_ = source.x_;
		y_.createEmpty(x_.nItems(), 0.0);
	}

	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		msg.error("Can't += these Data2D together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return;
	}

	// Add data together
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			msg.error("Can't += these Data2D, since the x axes are different.\n");
			return;
		}
		y_[n] += source.y_.value(n);
	}

	splineBracketLeft_ = -1;
}

/*!
 * \brief Operator *=
 */
void Data2D::operator*=(const double factor)
{
	// Multiply current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] *= factor;
	splineBracketLeft_ = -1;
}

/*!
 * \brief Operator /=
 */
void Data2D::operator/=(const double factor)
{
	// Divide current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] /= factor;
	splineBracketLeft_ = -1;
}

/*
// Fourier Transforms
*/

// Window Function keywords
const char *WindowFunctionKeywords[] = { "None", "Bartlett", "Gaussian", "Lanczos", "Nuttall", "Sine" };

/*!
 * \brief Convert text string to WindowFunction
 */
Data2D::WindowFunction Data2D::windowFunction(const char* s)
{
	for (int n=0; n<Data2D::nWindowFunctions; ++n) if (strcmp(s,WindowFunctionKeywords[n]) == 0) return (Data2D::WindowFunction) n;
	return Data2D::nWindowFunctions;
}

/*!
 * \brief Convert WindowFunction to text string
 */
const char* Data2D::windowFunction(Data2D::WindowFunction wf)
{
	return WindowFunctionKeywords[wf];
}

/*!
 * \brief Make some checks before doing transform
 */
bool Data2D::checkBeforeTransform()
{
	// Enough data to do transform?
	if (x_.nItems() < 5)
	{
		msg.error("Not enough X data points (%i) in Data2D. Can't do transform.\n", x_.nItems());
		return FALSE;
	}
	
	// X and Y arrays match?
	if (x_.nItems() != y_.nItems())
	{
		msg.error("X and Y array sizes do not match (%i vs %i) in Data2D. Can't do transform.\n", x_.nItems(), y_.nItems());
		return FALSE;
	}

	// Check spacing of data...
	double deltaX = x_[1] - x_[0], tolerance = 0.001;
	for (int n=2; n<x_.nItems(); ++n) if (fabs((x_[n] - x_[n-1])-deltaX) > tolerance)
	{
		msg.error("Data are unevenly spaced in Data2D. Can't do transform.\n");
		return FALSE;
	}
	
	return TRUE;
}

/*!
 * \brief Return value of window function at specified position (in range 0.0 - 1.0)
 * \details Calculates the value of the specified window function at the given value of 'x', where the range
 * of 'x' is between 0.0 and 1.0 inclusive, and represents the positive half of the waveform / window function.
 */
double Data2D::window(Data2D::WindowFunction wf, double x)
{
#ifdef CHECKS
	if ((x < 0.0) || (x > 1.0)) msg.warn("Position for window function is out of range (%f).\n", x);
#endif
	switch (wf)
	{
		case (Data2D::NoWindow):
			return 1.0;
			break;
		case (Data2D::BartlettWindow):
			return (1.0 - fabs( (x*0.5)/0.5 ));
			break;
		case (Data2D::GaussianWindow):
			return exp(-10.0*x*x);
			break;
		case (Data2D::LanczosWindow):
			return sin(PI*x)/(PI*x);
			break;
		case (Data2D::NuttallWindow):
			return (0.355768 - 0.487396*cos(2.0*PI*x) + 0.144232*cos(4.0*PI*x) - 0.012604*cos(6.0*PI*x));
			break;
		case (Data2D::SineWindow):
			return sin(PI*x);
			break;
		default:
			msg.error("Window function %i not implemented.\n", wf);
	}
	return 0.0;
}

/*!
 * \brief Perform plain Fourier transform of real data
 * \details Forward or backward Fourier transform the current data.
 */
bool Data2D::fourierTransformReal(bool forwardTransform, Data2D::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return FALSE;

	// Assume that the entire dataset constitutes one period of the function...
	double factor = (forwardTransform ? 1.0 : -1.0);
	double lambda = x_.last() - x_.first();
	double k = TWOPI / lambda;
	double deltaX = x_[1] - x_[0];
	msg.printVerbose("In Data2D::fourierTransformReal(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaX, k);

	// Create working arrays
	Array<double> real(x_.nItems()), imaginary(x_.nItems());
	real = 0.0;
	imaginary = 0.0;
	
	// Calculate complex Fourier coefficients
	double b, cosb, sinb;
	int n, m, nPoints = x_.nItems();
	for (n=0; n<nPoints-1; ++n)
	{
		for (m=1; m<nPoints; ++m)
		{
			b = (n+0.5)*k*x_[m];
			
			// Apply window function (Bartlett)
// 			b *= 1.0 - fabs( ((m/double(nPoints))*0.5)/0.5 );

			cosb = cos(b);
			sinb = sin(b) * factor;
			real[n] += y_[m] * cosb;
			imaginary[n] -= y_[m] * sinb;
		}
	}

	// Normalise coefficients (forward transform only)
	if (forwardTransform)
	{
		// All but zeroth term are multiplied by 2.0
		real[0] = real[0] / nPoints;
		imaginary[0] = imaginary[0] / nPoints;
		for (n=1; n<nPoints; ++n)
		{
			real[n] = 2.0 * real[n] / nPoints;
			imaginary[n] = 2.0 * imaginary[n] / nPoints;
		}
	}

	// Copy transform data over initial data
	for (n=0; n<nPoints; ++n) x_[n] = (n*0.5)*k;
	y_ = real;
	splineBracketLeft_ = -1;
	return TRUE;
}

/*!
 * \brief Transform g(r) to S(Q)
 */
bool Data2D::transformRDF(double atomicDensity, Data2D::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return FALSE;

	// Assume that the entire dataset constitutes one period of the function...
	// X values of original function are half-bin values, so we must add another bin width on to recover period of original function
	double lambda = x_.last() - x_.first() + 2.0*x_.first();
	double k = TWOPI / lambda;
	double deltaX = x_[1] - x_[0];
	double windowPos;
	msg.printVerbose("In Data2D::transformRDF(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaX, k);

	// Create working arrays
	Array<double> real(x_.nItems());
	real = 0.0;

	// Perform Fourier sine transform
	double Q, factor;
	int n, m, nPoints = x_.nItems();
	for (n=0; n<nPoints; ++n)
	{
		Q = (n+0.5)*k;
		for (m=0; m<nPoints; ++m)
		{
			windowPos = double(m) / double(nPoints-1);

// 			real[n] += x_[m]*x_[m]*y_[m] * sin(x_[m]*Q) * deltaX / (Q * x_[m]);
			real[n] += sin(x_[m]*Q) * x_[m] * window(wf, windowPos) * y_[m] * deltaX;
		}

		// Normalise
		factor = 4.0 * PI * atomicDensity / Q;
		real[n] *= factor;
	}

	// Copy transform data over initial data
	for (n=0; n<nPoints; ++n) x_[n] = (n+0.5)*k;
	y_ = real;
	splineBracketLeft_ = -1;
	return TRUE;
}

/*!
 * \brief Transform g(r) to S(Q), applying instrumental broadening functions
 */
bool Data2D::transformBroadenedRDF(double atomicDensity, double qStep, double fwhm, double fwhmq, Data2D::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return FALSE;

	// Assume that the entire dataset constitutes one period of the function...
	// X values of original function are half-bin values, so we must add another bin width on to recover period of original function
	double lambda = x_.last() - x_.first() + 2.0*x_.first();
	double k = TWOPI / lambda;
	double deltaX = x_[1] - x_[0];
	double windowPos, broaden, sigma, sigmaq, sigr, Q, factor, qMax, fq;
	int n, m, nR = x_.nItems();
	msg.printVerbose("In Data2D::transformBroadenedRDF(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaX, k);

	sigma = 0.5*fwhm/sqrt(2.0*log(2.0));
	sigmaq = 0.5*fwhmq/sqrt(2.0*log(2.0));

	// Create working arrays
	Array<double> real;
	Q = qStep*0.5;
	qMax = (nR-0.5)*k;

	// Perform Fourier sine transform, including instrument broadening of RDF
	while (Q <= qMax)
	{
		fq = 0.0;
		for (m=0; m<nR; ++m)
		{
			// Get window value at this position in the function
			windowPos = double(m) / double(nR-1);

			// Calculate broadening
			sigr = (sigma + sigmaq*Q) * x_[m];
			broaden = exp(-0.5*sigr*sigr);

			fq += sin(x_[m]*Q) * x_[m] * broaden * window(wf, windowPos) * y_[m] * deltaX;
		}

		// Normalise
		factor = 4.0 * PI * atomicDensity / Q;
		real.add(fq*factor);
		
		Q += qStep;
	}

	// Copy transform data over initial data
	y_ = real;
	x_.clear();
	for (n=0; n<y_.nItems(); ++n) x_.add((n+0.5)*qStep);
	splineBracketLeft_ = -1;
	return TRUE;
}

/*!
 * \brief Transform S(Q) to g(r)
 */
bool Data2D::transformSQ(double atomicDensity, Data2D::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return FALSE;

	// Assume that the entire dataset constitutes one period of the function...
	// X values of original function are half-bin values, so we must add another bin width on to recover period of original function
	double lambda = x_.last() - x_.first() + 2.0*x_.first();
	double k = TWOPI / lambda;
	double deltaQ = x_[1] - x_[0];
	double windowPos;
	msg.printVerbose("In Data2D::transformSQ(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaQ, k);

	// Create working arrays
	Array<double> real(x_.nItems());
	real = 0.0;
	
	// Perform Fourier sine transform
	double r, factor;
	int n, m, nPoints = x_.nItems();
	for (n=0; n<nPoints; ++n)
	{
		r = (n+0.5)*k;
		for (m=0; m<nPoints; ++m)
		{
			windowPos = double(m) / double(nPoints-1);
			
// 			real[n] += x_[m]*x_[m]*y_[m] * sin(x_[m]*Q) * deltaX / (Q * x_[m]);
			real[n] += sin(x_[m]*r) * x_[m] * window(wf, windowPos) * y_[m] * deltaQ;
		}

		// Normalise
		factor = 1.0 / (2.0 * PI * PI * atomicDensity * r);
// 		factor = 0.5*deltaQ/PI/PI/atomicDensity;
		real[n] *= factor;
	}

	// Copy transform data over initial data
	for (n=0; n<nPoints; ++n) x_[n] = (n+0.5)*k;
	y_ = real;
	splineBracketLeft_ = -1;
	return TRUE;
}

/*!
 * \brief Calculate S(Q) correlation function
 */
bool Data2D::correlateSQ(double atomicDensity)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return FALSE;

	// Assume that the entire dataset constitutes one period of the function...
	double lambda = x_.last() - x_.first();
	double k = TWOPI / lambda;
	double deltaQ = x_[1] - x_[0];
	msg.printVerbose("In Data2D::correlateSQ(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaQ, k);

	// Create working arrays
	Array<double> real(x_.nItems());
	real = 0.0;
	
	// Perform Fourier sine transform
	double r, factor;
	int n, m, nPoints = x_.nItems();
	for (n=0; n<nPoints; ++n)
	{
		r = (n+0.5)*k;
		for (m=0; m<nPoints; ++m) real[n] += sin(x_[m]*r) * x_[m] * (1.0 - (1.0/y_[m])) * deltaQ;

		// Normalise
		factor = 1.0 / (2.0 * PI * PI * atomicDensity * r);
		real[n] *= factor;
	}

	// Copy transform data over initial data
	for (n=0; n<nPoints; ++n) x_[n] = (n+0.5)*k;
	y_ = real;
	splineBracketLeft_ = -1;
	return TRUE;
}

/*
// Spline Interpolation
*/

/*!
 * \brief Calculate natural spline interpolation of current data
 */
void Data2D::interpolate()
{
	// Initialise second derivative and working arrays
	int nPoints = x_.nItems();
	ddy_.createEmpty(nPoints);
	Array<double> u(nPoints);

	// Set first and last second derivatives to zero (giving a natural spline fit)
	ddy_[0] = 0.0;
	ddy_[nPoints-1] = 0.0;
	u[0] = 0.0;
	u[nPoints-1] = 0.0;

	double B, p;
	// Loop over data points 1 to N-2
	for (int n=1; n<nPoints-2; ++n)
	{
		B = (x_[n] - x_[n-1]) / (x_[n+1] - x_[n-1]);
		p = B*ddy_[n-1] + 2.0;
		ddy_[n] = (B-1.0)/p;
		u[n] = (y_[n+1] - y_[n]) / (x_[n+1] - x_[n]) - (y_[n] - y_[n-1]) / (x_[n] - x_[n-1]);
		u[n] = (6.0*u[n]/(x_[n+1] - x_[n-1]) - B*u[n-1]) / p;
	}
	
	for (int n=nPoints-2; n>=0; --n) ddy_[n] = ddy_[n]*ddy_[n+1] + u[n];
	
	splineBracketLeft_ = 0;
	splineBracketRight_ = 0;
}

/*!
 * \brief Return second derivatives array
 */
Array<double>& Data2D::splineArray()
{
	return ddy_;
}

/*!
 * \brief Return spline interpolated y value for supplied x
 */
double Data2D::interpolated(double xvalue)
{
	// Do we need to (re)generate the interpolation?
	if (splineBracketLeft_ == -1) interpolate();
	
	// X and ddy arrays match?
	if (x_.nItems() != ddy_.nItems())
	{
		msg.error("X and ddY array sizes do not match (%i vs %i) in Data2D::interpolated().\n", x_.nItems(), ddy_.nItems());
		return 0.0;
	}

	// Bracket x-value - we'll assume that access is likely to be more sequential than random...
	// Check previous limits...
	if (splineBracketLeft_ != splineBracketRight_)
	{
		if ((x_[splineBracketLeft_] > xvalue) || (x_[splineBracketRight_] < xvalue))
		{
			splineBracketLeft_ = 0;
			splineBracketRight_ = 0;
		}
	}
	
	// Still need new limits?
	if (splineBracketLeft_ == splineBracketRight_)
	{
		splineBracketLeft_ = 0;
		splineBracketRight_ = ddy_.nItems()-1;
		int i;
		while ((splineBracketRight_-splineBracketLeft_) > 1)
		{
			i = (splineBracketRight_+splineBracketLeft_) / 2;
			if (x_[i] > xvalue) splineBracketRight_ = i;
			else splineBracketLeft_ = i;
		}
	}
	
	// Calculate interpolated point
	double a, b, interval = x_[splineBracketRight_] - x_[splineBracketLeft_];
	a = (x_[splineBracketRight_] - xvalue) / interval;
	b = (xvalue - x_[splineBracketLeft_]) / interval;
	
	// Calculate cubic polynomial
	double result = a*y_[splineBracketLeft_] + b*y_[splineBracketRight_] + ((a*a*a-a)*ddy_[splineBracketLeft_] + (b*b*b-b)*ddy_[splineBracketRight_])*(interval*interval)/6.0;
	return result;
}

/*!
 * \brief Smooth data
 */
void Data2D::smooth(int avgSize, int skip)
{
	// First, create a new dataset using Y-averages of the current data
	Data2D avg;
	double y;
	int n, m, i = avgSize/2;
	for (n=i; n < x_.nItems()-i; n += (1+skip))
	{
		y = 0.0;
		for (m=n-i; m <= n+i; ++m) y += y_[m];
		y /= avgSize;
		
		avg.addPoint(x_[n], y);
	}

	avg.interpolate();

	// Now go through old data, setting new Y values from the interpolation
	for (n=0; n<x_.nItems(); ++n) y_[n] = avg.interpolated(x_[n]);
}

/*!
 * \brief Add interpolated data
 */
void Data2D::addInterpolated(Data2D& source)
{
	for (int n=0; n<x_.nItems(); ++n) addY(n, source.interpolated(x_.value(n)));
}

/*!
 * \brief Subtract interpolated data
 */
void Data2D::subtractInterpolated(Data2D& source)
{
	for (int n=0; n<x_.nItems(); ++n) addY(n, -source.interpolated(x_.value(n)));
}

/*
// General Functions
*/

/*!
 * \brief Return minumum x value in data
 */
double Data2D::xMin()
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_[0];
	for (int n=1; n<x_.nItems(); ++n) if (x_[n] < result) result = x_[n];
	return result;
}

/*!
 * \brief Return maxumum x value in data
 */
double Data2D::xMax()
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_[0];
	for (int n=1; n<x_.nItems(); ++n) if (x_[n] > result) result = x_[n];
	return result;
}

/*!
 * \brief Return minumum y value in data
 */
double Data2D::yMin()
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_[0];
	for (int n=1; n<y_.nItems(); ++n) if (y_[n] < result) result = y_[n];
	return result;
}

/*!
 * \brief Return maxumum y value in data
 */
double Data2D::yMax()
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_[0];
	for (int n=1; n<y_.nItems(); ++n) if (y_[n] > result) result = y_[n];
	return result;
}

/*!
 * \brief Compute integral of the data
 * \details Compute the integral of the current data using the Trapezium method
 */
double Data2D::integral()
{
	double total = 0.0, y0 = y_.first(), y1, x0 = x_.first(), x1;
	for (int n=1; n<x_.nItems(); ++n)
	{
		x1 = x_[n];
		y1 = y_[n];
		total += (x1 - x0) * (y0 + y1) * 0.5;
		x0 = x1;
		y0 = y1;
	}
	return total;
}

/*!
 * \brief Compute absolute integral of the data
 * \details Compute the absolute integral of the current data using the Trapezium method
 */
double Data2D::absIntegral()
{
	double total = 0.0, y0 = y_.first(), y1, x0 = x_.first(), x1;
	for (int n=1; n<x_.nItems(); ++n)
	{
		x1 = x_[n];
		y1 = y_[n];
		total += fabs((x1 - x0) * (y0 + y1) * 0.5);
		x0 = x1;
		y0 = y1;
	}
	return total;
}

/*!
 * \brief Apply median filter to data
 */
void Data2D::medianFilter(int length)
{
	double data[length], avg, result;
	int m, i = length/2, n = length/2, miny, maxy;
	Array<double> newY(x_.nItems());
	
	// Set boundary values
	for (m=0; m<n; ++m)
	{
		newY[m] = y_[m];
		newY[x_.nItems()-1-m] = y_[m];
	}

	// Now loop over remaining points
	while (n < (x_.nItems()-i))
	{
		// Grab data values, and determine min/max values
		miny = 0;
		maxy = 0;
		for (m=-i; m<=i; ++m)
		{
			data[m+i] = y_[n+m];
			if (data[m+i] < data[miny]) miny = m+i;
			if (data[m+i] > data[maxy]) maxy = m+i;
		}

		// Determine median value (without sorting)
		// First, calculate average value
		avg = 0.0;
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy)) avg += data[m];
		avg /= length-2;
		// Now find value closest to the average
		result = data[0];
		for (m=0; m<length; ++m) if ((m != miny) && (m != maxy) && (fabs(data[m]-avg) < fabs(result-avg))) result = data[m];
		
		// Store median value
		newY[n] = result;
		
		++n;
	}
	
	// Store new values
	y_ = newY;
}

/*!
 * \brief Convolute this data with the supplied data, by products
 */
bool Data2D::convoluteProduct(Data2D& data)
{
	// Check compatibility of datasets
	if (data.nPoints() != nPoints())
	{
		msg.error("Refusing to convolute by product two datasets of different sizes.\n");
		return FALSE;
	}
	for (int n=0; n<nPoints(); ++n)
	{
		if (fabs(data.x(n) - x_[n]) > 1.0e-5)
		{
			msg.error("Refusing to convolute by product two datasets with different x-values.\n");
			return FALSE;
		}
	}

	// Ready to go...
	for (int n=0; n<nPoints(); ++n) y_[n] *= data.y(n);
	
	return TRUE;
}

/*
// File I/O
*/

/*!
 * \brief Load data from specified file
 */
bool Data2D::load(const char* fileName)
{
	// Only the Master process can do this
	if (Comm.slave()) return TRUE;

	// Open file and check that we're OK to proceed reading from it
	LineParser parser;

	if ((!parser.openInput(fileName)) || (!parser.isFileGoodForReading()))
	{
		msg.error("Couldn't open file '%s' for reading.\n", fileName);
		return FALSE;
	}

	int success, nCols = -1;
	clear();
	while (!parser.eofOrBlank())
	{
		success = parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks);
		if (success != 0)
		{
			parser.closeFiles();
			msg.error("Error reading from file '%s'.\n", fileName);
			return FALSE;
		}

		addPoint(parser.argd(0), parser.argd(1));
	}
	
	parser.closeFiles();
	
	if (nCols == 3) msg.print("Loaded %i points from file '%s' (including spline coefficients).\n", nPoints(), fileName);
	else msg.print("Loaded %i points from file '%s'.\n", nPoints(), fileName);
	
	return TRUE;
}

/*!
 * \brief Save data to specified file
 */
bool Data2D::save(const char* fileName) const
{
	// Only the Master process can do this
	if (Comm.slave()) return TRUE;

	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing datafile '%s'...\n", fileName);

	parser.openOutput(fileName, TRUE);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return FALSE;
	}
	
	for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e\n", x_.value(n), y_.value(n));
	parser.closeFiles();
	return TRUE;
}

/*!
 * \brief Save data and interpolation to specified file
 */
bool Data2D::saveWithInterpolation(const char* fileName)
{
	// Only the Master process can do this
	if (Comm.slave()) return TRUE;

	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
	msg.print("Writing datafile '%s'...\n", fileName);

	parser.openOutput(fileName, TRUE);
	if (!parser.isFileGoodForWriting())
	{
		msg.error("Couldn't open file '%s' for writing.\n", fileName);
		return FALSE;
	}
	
	for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e\n", x_.value(n), y_.value(n), interpolated(x_.value(n)));
	parser.closeFiles();
	return TRUE;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data
 */
bool Data2D::broadcast()
{
#ifdef PARALLEL
	// X data
	int nItems = x_.nItems();
	if (!Comm.broadcast(&nItems, 1)) return FALSE;
	if (nItems > 0)
	{
		if (Comm.slave()) x_.createEmpty(nItems);
		if (!Comm.broadcast(x_.array(), nItems)) return FALSE;
	}

	// Y data
	nItems = y_.nItems();
	if (!Comm.broadcast(&nItems, 1)) return FALSE;
	if (nItems > 0)
	{
		if (Comm.slave()) y_.createEmpty(nItems);
		if (!Comm.broadcast(y_.array(), nItems)) return FALSE;
	}

	// Spline data
	nItems = ddy_.nItems();
	if (!Comm.broadcast(&nItems, 1)) return FALSE;
	if (nItems > 0)
	{
		if (Comm.slave()) ddy_.createEmpty(nItems);
		if (!Comm.broadcast(ddy_.array(), nItems)) return FALSE;
	}
	if (!Comm.broadcast(&splineBracketLeft_, 1)) return FALSE;
	if (!Comm.broadcast(&splineBracketRight_, 1)) return FALSE;

	// Axis/title information
	if (!Comm.broadcast(name_)) return FALSE;
#endif
	return TRUE;
}
