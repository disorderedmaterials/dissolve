/*
	*** Basic XY Data
	*** src/base/xydata.cpp
	Copyright T. Youngs 2012-2017

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

#include "base/processpool.h"
#include "base/xydata.h"
#include "base/messenger.h"
#include "math/constants.h"
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

// Static Members
bool XYData::useFFT_ = false;

// Constructor
XYData::XYData() : ListItem<XYData>()
{
	name_ = "Untitled";
	splineInterval_ = -1;
	constrainedSpline_ = true;
}

// Destructor
XYData::~XYData()
{
}

// Copy Constructor
XYData::XYData(const XYData& source)
{
	x_ = source.x_;
	y_ = source.y_;
	splineA_ = source.splineA_;
	splineB_ = source.splineB_;
	splineC_ = source.splineC_;
	splineD_ = source.splineD_;
	splineH_ = source.splineH_;
	splineInterval_ = source.splineInterval_;
	constrainedSpline_ = source.constrainedSpline_;
	name_ = source.name_;
}

// Clear Data
void XYData::clear()
{
	x_.clear();
	y_.clear();
	splineA_.clear();
	splineB_.clear();
	splineC_.clear();
	splineD_.clear();
	splineH_.clear();
	splineInterval_ = -1;
}

/*
 * Data
 */

// Reset arrays to zero
void XYData::reset()
{
	for (int n=0; n<x_.nItems(); ++n) x_[n] = 0.0;
	for (int n=0; n<y_.nItems(); ++n) y_[n] = 0.0;
	splineInterval_ = -1;
}

// Initialise arrays to specified size
void XYData::initialise(int size)
{
	clear();
	x_.initialise(size);
	y_.initialise(size);
}

// Create new X data and empty Y data
void XYData::createEmpty(double xDelta, double xMax, bool halfBins)
{
	clear();
	double x = (halfBins ? 0.5*xDelta : 0.0);
	while (x <= xMax)
	{
		addPoint(x, 0.0);
		x += xDelta;
	}
}

// Copy existing X and Y data
void XYData::copyData(XYData& source)
{
	x_ = source.x_;
	y_ = source.y_;
	splineA_ = source.splineA_;
	splineB_ = source.splineB_;
	splineC_ = source.splineC_;
	splineD_ = source.splineD_;
	splineH_ = source.splineH_;
	splineInterval_ = source.splineInterval_;
	constrainedSpline_ = source.constrainedSpline_;
}

// Copy existing X data and generate empty Y
void XYData::templateFrom(XYData& source)
{
	x_ = source.x_;
	y_.initialise(x_.nItems(), 0.0);
	splineInterval_ = -1;
}

// Return current array size
int XYData::arraySize()
{
	return x_.nItems();
}

// Set specified data point
void XYData::setPoint(int index, double x, double y)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::setPoint().\n", index);
		return;
	}
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::setPoint().\n", index);
		return;
	}
#endif
	x_[index] = x;
	y_[index] = y;
	splineInterval_ = -1;
}

// Return number of defined datapoints
int XYData::nPoints() const
{
	return x_.nItems();
}

// Set x value
void XYData::setX(int index, double x)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::setX().\n", index);
		return;
	}
#endif
	x_[index] = x;
	splineInterval_ = -1;
}

// Add to x value
void XYData::addX(int index, double delta)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::addX().\n", index);
		return;
	}
#endif
	x_[index] += delta;
	splineInterval_ = -1;
}

// Return x value specified
double XYData::x(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::x().\n", index);
		return 0.0;
	}
#endif
	return x_.value(index);
}

// Return x Array
Array<double>& XYData::arrayX()
{
	splineInterval_ = -1;
	return x_;
}

// Set y value
void XYData::setY(int index, double y)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::setY().\n", index);
		return;
	}
#endif
	y_[index] = y;
	splineInterval_ = -1;
}

// Add to y value
void XYData::addY(int index, double delta)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::addY().\n", index);
		return;
	}
#endif
	y_[index] += delta;
	splineInterval_ = -1;
}

// Add to y array
bool XYData::addY(const Array<double>& source, double factor)
{
	if (y_.nItems() != source.nItems())
	{
		Messenger::print("BAD_USAGE - Can't add Y values from source array, since the number of items differs (%i vs %i).\n", y_.nItems(), source.nItems());
		return false;
	}

	for (int n=0; n<y_.nItems(); ++n) y_[n] += source.value(n)*factor;
	splineInterval_ = -1;
	return true;
}

// Multiply y value
void XYData::multiplyY(int index, double factor)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::multiplyY().\n", index);
		return;
	}
#endif
	y_[index] *= factor;
	splineInterval_ = -1;
}

// Return y value specified
double XYData::y(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::print("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::y().\n", index);
		return 0.0;
	}
#endif
	return y_.value(index);
}

// Return y Array
Array<double>& XYData::arrayY()
{
	splineInterval_ = -1;
	return y_;
}

// Add new data point
void XYData::addPoint(double x, double y)
{
	x_.add(x);
	y_.add(y);
	splineInterval_ = -1;
}

// Set name of data
void XYData::setName(const char* name)
{
	name_ = name;
}

// Return name of data
const char* XYData::name() const
{
	return name_.get();
}

/*
 * Operators
 */

// Operator =
void XYData::operator=(const XYData& source)
{
	x_ = source.x_;
	y_ = source.y_;
	splineA_ = source.splineA_;
	splineB_ = source.splineB_;
	splineC_ = source.splineC_;
	splineD_ = source.splineD_;
	splineH_ = source.splineH_;
	splineInterval_ = source.splineInterval_;
	name_ = source.name_;
}

// Operator +
XYData XYData::operator+(const XYData& source) const
{
	// Construct copy of current data
	XYData newData(*this);
	
	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't + these XYData together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return newData;
	}

	// Add data together
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't + these XYData, since the x axes are different.\n");
			return newData;
		}
		newData.y_[n] += source.y_.value(n);
	}
	
	newData.splineInterval_ = -1;
	
	return newData;
}

// Operator +=
void XYData::operator+=(const XYData& source)
{
	// If source array is empty, nothing to do
	if (source.nPoints() == 0) return;

	// Initialise current arrays?
	if (x_.nItems() == 0)
	{
		x_ = source.x_;
		y_.initialise(x_.nItems(), 0.0);
	}

	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't += these XYData together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return;
	}

	// Add data together
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't += these XYData, since the x axes are different.\n");
			return;
		}
		y_[n] += source.y_.value(n);
	}

	splineInterval_ = -1;
}

// Operator +=
void XYData::operator+=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] += dy;
	splineInterval_ = -1;
}

// Operator -
XYData XYData::operator-(const XYData& source) const
{
	// Construct copy of current data
	XYData newData(*this);
	
	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't - these XYData together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return newData;
	}

	// Subtact source data from this data
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't - these XYData, since the x axes are different.\n");
			return newData;
		}
		newData.y_[n] -= source.y_.value(n);
	}
	
	newData.splineInterval_ = -1;
	
	return newData;
}

// Operator -=
void XYData::operator-=(const XYData& source)
{
	// Initialise current arrays?
	if (x_.nItems() == 0)
	{
		x_ = source.x_;
		y_.initialise(x_.nItems(), 0.0);
	}

	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't -= these XYData together, since they are of differing sizes (%i vs %i).\n", x_.nItems(), source.x_.nItems());
		return;
	}

	// Subtact source data from this
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Check x values
		if (fabs(x_.value(n) - source.x_.value(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't -= these XYData, since the x axes are different.\n");
			return;
		}
		y_[n] -= source.y_.value(n);
	}

	splineInterval_ = -1;
}

// Operator -=
void XYData::operator-=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] -= dy;
	splineInterval_ = -1;
}

// Operator *=
void XYData::operator*=(const double factor)
{
	// Multiply current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] *= factor;
	splineInterval_ = -1;
}

// Operator /=
void XYData::operator/=(const double factor)
{
	// Divide current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] /= factor;
	splineInterval_ = -1;
}

/*
 * Fourier Transforms
 */

// Window Function keywords
const char* WindowFunctionKeywords[] = { "None", "Bartlett", "Hann", "Lanczos", "Nuttall", "Sine" };

// Convert text string to WindowFunction
XYData::WindowFunction XYData::windowFunction(const char* s)
{
	for (int n=0; n<XYData::nWindowFunctions; ++n) if (strcmp(s,WindowFunctionKeywords[n]) == 0) return (XYData::WindowFunction) n;
	return XYData::nWindowFunctions;
}

// Convert WindowFunction to text string
const char* XYData::windowFunction(XYData::WindowFunction wf)
{
	return WindowFunctionKeywords[wf];
}

// Make some checks before doing transform
bool XYData::checkBeforeTransform()
{
	// Enough data to do transform?
	if (x_.nItems() < 5)
	{
		Messenger::error("Not enough X data points (%i) in XYData. Can't do transform.\n", x_.nItems());
		return false;
	}
	
	// X and Y arrays match?
	if (x_.nItems() != y_.nItems())
	{
		Messenger::error("X and Y array sizes do not match (%i vs %i) in XYData. Can't do transform.\n", x_.nItems(), y_.nItems());
		return false;
	}

	// Check spacing of data...
	double deltaX = x_[1] - x_[0], tolerance = 0.001;
	for (int n=2; n<x_.nItems(); ++n) if (fabs((x_[n] - x_[n-1])-deltaX) > tolerance)
	{
		Messenger::error("Data are unevenly spaced in XYData. Can't do transform.\n");
		return false;
	}
	
	return true;
}

// Return value of window function at specified position (in range 0.0 - 1.0 inclusive)
double XYData::window(XYData::WindowFunction wf, double x)
{
#ifdef CHECKS
	if ((x < 0.0) || (x > 1.0)) Messenger::warn("Position for window function is out of range (%f).\n", x);
#endif
	switch (wf)
	{
		case (XYData::NoWindow):
			return 1.0;
			break;
		case (XYData::BartlettWindow):
			return (1.0 - fabs( (x-0.5)/0.5 ));
			break;
		case (XYData::HannWindow):
			return 0.5*(1.0-cos(2*PI*x));
			break;
		case (XYData::LanczosWindow):
			return sin(PI*(2*x-1.0))/(PI*(2*x-1.0));
			break;
		case (XYData::NuttallWindow):
			return (0.355768 - 0.487396*cos(2.0*PI*x) + 0.144232*cos(4.0*PI*x) - 0.012604*cos(6.0*PI*x));
			break;
		case (XYData::SineWindow):
			return sin(PI*x);
			break;
		default:
			Messenger::error("Window function %i not implemented.\n", wf);
	}
	return 0.0;
}

// Perform plain Fourier transform of real data
bool XYData::fourierTransformReal(bool forwardTransform, XYData::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	// Assume that the entire dataset constitutes one period of the function...
	double factor = (forwardTransform ? 1.0 : -1.0);
	double lambda = x_.last() - x_.first();
	double k = TWOPI / lambda;
	double deltaX = x_[1] - x_[0];
	Messenger::printVerbose("In XYData::fourierTransformReal(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaX, k);

	// Create working arrays
	Array<double> real(x_.nItems()), imaginary(x_.nItems());
	real = 0.0;
	imaginary = 0.0;
	
	// Calculate complex Fourier coefficients
	double b, cosb, sinb, winFunc;
	int n, m, nPoints = x_.nItems();
	for (n=0; n<nPoints; ++n)
	{
		for (m=0; m<nPoints; ++m)
		{
			b = n * TWOPI * m / nPoints;
			winFunc = window(wf, double(m) / nPoints);

			cosb = cos(b);
			sinb = sin(b) * factor;
			real[n] += y_[m] * winFunc * cosb;
			imaginary[n] -= y_[m] * winFunc * sinb;
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
	splineInterval_ = -1;
	return true;
}

// Transform g(r) to S(Q)
bool XYData::transformRDF(double atomicDensity, XYData::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	// Assume that the entire dataset constitutes one period of the function...
	// Assume X values of original function are central bin values, so we must add another bin width on to recover period of original function
	double deltaR = x_[1] - x_[0];
	double lambda = x_.last() - x_.first() + deltaR;
	double k = TWOPI / lambda;
	double windowPos;
	Messenger::printVerbose("In XYData::transformRDF(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaR, k);

	if (XYData::useFFT_)
	{
		// Pad input data up to power of 2 and bit-reverse data
		int n, i, j, bit, N = pow( 2, ceil( log( x_.nItems() ) / log( 2 ) ) );
		printf("Original input size = %i, pow2 = %i\n", x_.nItems(), N);
		double temp;
		Array<double> yin(N), xin(N);
		for (n=0; n<y_.nItems(); ++n)
		{
			yin[n] = y_[n]; // * window(wf, double(n)/y_.nItems());
			xin[n] = x_[n];
		}
		for (n=y_.nItems(); n<N; ++n)
		{
			yin[n] = 0.0;
			xin[n] = 0.0;
		}
		// -- Perform bit-reverse in-place in real[]
		j = 0;
		for (i=0; i<N; ++i)
		{
			// Swap values only if i < j, avoiding unnecessary symmetric and equivalent (i == j) swaps
			if (i < j)
			{
// 				printf("Swapping i = ");
// 				for (int m=1; m<=N; m<<=1) printf("%i", (i & m) ? 1 : 0); printf("  (%i) for j = ", i);
// 				for (int m=1; m<=N; m<<=1) printf("%i", (j & m) ? 1 : 0); printf("  (%i)\n", j);
				temp = yin[i];
				yin[i] = yin[j];
				yin[j] = temp;
				temp = xin[i];
				xin[i] = xin[j];
				xin[j] = temp;
			}
			// Increment j in bit-reversed order
			bit = N;
			do
			{
				bit >>= 1;
				j ^= bit;
			} while( (j & bit) == 0 && bit != 1 );
		}

// 		for (n=0; n<N; ++n) printf("Array[%i] : Orig = %f   new = %f\n", n, y_[n], yin[n]);

		// Perform FFT
		int npoint=1, istep, m;
		double theta, wpr, wr, wi, tempr, wpi, wtemp;
// 		double b, cosb, tempr;
		// -- Loop over successive N-point transforms, up to and including N/2
		while (npoint < N)
		{
			// Determine spacing to use between points in array
			istep = 2*npoint;
// 			printf("MMax = %i, IStep = %i\n", npoint, istep); 

			// Calculate current theta
			theta = 1.0*(TWOPI/istep);
			// Initialize the trigonometric recurrence.
			wtemp = sin(0.5*theta);
			wpr = -2.0*wtemp*wtemp;
			wr = 1.0;

			// Loop over 
			for (m=0; m<npoint; ++m)
			{
// 				printf("Middle loop : m = %i, W = %f\n", m+1, wr);
				wr = cos(m*TWOPI/istep);
				// Here are the two nested inner loops.
				for (i=m; i<N; i+=istep)
				{
// 					b = (m+0.5)*npoint*k*xin[i];

					j = i+npoint;
// 					printf("  inner loop i = %i j = %i, f(i) = %f\n", i+1, j+1, yin[i]);
					// This is the Danielson-Lanczos formula:
// 					tempr=wr*data[j]-wi*data[j+1];
// 					tempi=wr*data[j+1]+wi*data[j];
					tempr = wr * yin[j];
// 					cosb = cosb) * yin[j];
					yin[j] = yin[i]-tempr;
// 					data[j+1]=data[i+1]-tempi;
					yin[i] += tempr;
// 					data[i+1] += tempi;
				}
				wr = wr*wpr + wr;
// 				wr = wr - wr * 2.0 * (0.5 - 0.5 * cos(TWOPI/m));
// 				wr *= cos(TWOPI/npoint);
				// Trigonometric recurrence.
// 				wi=wi*wpr+wtemp*wpi+wi;
			}
			npoint = istep;
		}

// 		for (n=0; n<N; ++n) printf("FFT = %f\n", yin[n]);
		
		// Copy transform data over initial data
		for (n=0; n<x_.nItems(); ++n) x_[n] = (n+0.5)*k;
		for (n=0; n<x_.nItems(); ++n) y_[n] = yin[n];
		y_ *= 2.0 / x_.nItems();
		y_[0] / 2.0;
	}
	else
	{
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

	// 			real[n] += x_[m]*x_[m]*y_[m] * sin(x_[m]*Q) * deltaR / (Q * x_[m]);
				real[n] += sin(x_[m]*Q) * x_[m] * window(wf, windowPos) * y_[m] * deltaR;
			}

			// Normalise
			factor = 4.0 * PI * atomicDensity / Q;
			real[n] *= factor;
		}

		// Copy transform data over initial data
		for (n=0; n<nPoints; ++n) x_[n] = (n+0.5)*k;
		y_ = real;
	}

	splineInterval_ = -1;
	return true;
}

// Transform g(r) to S(Q), applying instrumental broadening functions
bool XYData::transformBroadenedRDF(double atomicDensity, double qStep, double qMax, double qDepFWHM, double qIndepFWHM, XYData::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	// Assume that the entire dataset constitutes one period of the function...
	// X values of original function are half-bin values, so we must add another bin width on to recover period of original function
	double lambda = x_.last() - x_.first() + 2.0*x_.first();
	double k = TWOPI / lambda;
	double deltaX = x_[1] - x_[0];
	double windowPos, broaden, sigma, sigmaq, sigr, Q, factor, fq;
	int n, m, nR = x_.nItems();
	Messenger::printVerbose("In XYData::transformBroadenedRDF(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaX, k);

	sigma = 0.5*qIndepFWHM/sqrt(2.0*log(2.0));
	sigmaq = 0.5*qDepFWHM/sqrt(2.0*log(2.0));

	// Create working arrays
	Array<double> oldy = y_;
	y_.forgetData();
	Q = qStep*0.5;

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

			fq += sin(x_[m]*Q) * x_[m] * broaden * window(wf, windowPos) * oldy[m] * deltaX;
		}

		// Normalise
		factor = 4.0 * PI * atomicDensity / Q;
		y_.add(fq*factor);
		
		Q += qStep;
	}

	// Copy transform data over initial data
	x_.forgetData();
	for (n=0; n<y_.nItems(); ++n) x_.add((n+0.5)*qStep);

	splineInterval_ = -1;

	return true;
}

// Transform S(Q) to g(r)
bool XYData::transformSQ(double atomicDensity, XYData::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	// Assume that the entire dataset constitutes one period of the function...
	// Assume that X values of original function are half-bin values, so we must add another bin width on to recover period of original function
	double deltaQ = x_[1] - x_[0];
	double lambda = x_.last() - x_.first() + lambda;
	double k = TWOPI / lambda;
	double windowPos;
	Messenger::printVerbose("In XYData::transformSQ(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaQ, k);

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
			
			real[n] += sin(x_[m]*r) * x_[m] * window(wf, windowPos) * y_[m] * deltaQ;
		}

		// Normalise
		factor = 1.0 / (2.0 * PI * PI * atomicDensity * r);
		real[n] *= factor;
	}

	// Copy transform data over initial data
	for (n=0; n<nPoints; ++n) x_[n] = (n+0.5)*k;
	y_ = real;

	splineInterval_ = -1;
	return true;
}

// Fourier transform current data, applying line-width broadening in real-space using the modified Lorch function
bool XYData::transformLorch(double atomicDensity, double step, double rMax, double beta, double delta0, bool qToR)
{
	/*
	 * Fourier transforms from Q to r-space, or r to Q-space, employing the modified Lorch function as described by Soper in
	 * XXX TODO.
	 * XXX TODO Only valid when input x_[] values are bin boundaries, not centre-bin values.
	 */

	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	double deltaIn = x_[1] - x_[0];
	double deltar, width, r, factor, norm, ftx, qr, j1qr, j2qr;
	int n, m, nPts = x_.nItems();

	// Create working arrays
	Array<double> result;
	r = step * 0.5;

	// Setup correct factor, depending on whether we are going from Q -> r or r -> Q
	if (qToR) factor = 1.0 / (2.0 * PI * PI * atomicDensity);
	else factor = 4.0 * PI * atomicDensity;
	
	// Perform Fourier sine transform, including line-broadening of RDF
	while (r <= rMax)
	{
		// Reset sum, and calculate r-dependent values
		ftx = 0.0;
		norm = factor / (r*r*r);
		deltar = delta0 * (1.0 + pow(r, beta));

		// Calculate first argument at left-hand bin boundary
		qr = r * x_[0];
		j1qr = sin(qr) - qr * cos(qr);

		for (m=0; m<nPts-1; ++m)
		{
			// Calculate second argument at right-hand bin boundary
			qr = r * x_[m+1];
			j2qr = sin(qr) - qr * cos(qr);

			// Calculate 'width' at centre of bin
			width = 0.5 * (x_[m]+x_[m+1]) * deltar;
            
			// Form integral of (Q sin Qr)/r from r1 to r2 (as detailed in equation 3.86 in reference above)
			// Modified Lorch function is constructed and applied if the width is non-zero and positive
			if (width < 0.0) ftx += y_[m] * (j2qr - j1qr);
			else ftx += y_[m] * (j2qr - j1qr) * (3.0/(width*width*width)) * (sin(width) - width*cos(width));

			// Overwrite first Lorch argument
			j1qr = j2qr;
		}

		// Sum
		result.add(ftx * norm);
		
		r += step;
	}

	// Copy transform data over initial data
	y_ = result;
	x_.forgetData();
	for (n=0; n<y_.nItems(); ++n) x_.add((n+0.5)*step);
	splineInterval_ = -1;
	return true;
}

// Calculate S(Q) correlation function
bool XYData::correlateSQ(double atomicDensity)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	// Assume that the entire dataset constitutes one period of the function...
	double lambda = x_.last() - x_.first();
	double k = TWOPI / lambda;
	double deltaQ = x_[1] - x_[0];
	Messenger::printVerbose("In XYData::correlateSQ(), period of function is %f, real deltaX is %f, and wavenumber is %f\n", lambda, deltaQ, k);

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
	splineInterval_ = -1;
	return true;
}

/*
 * Spline Interpolation
 */

// Calculate natural spline interpolation of current data
void XYData::interpolate(bool constrained)
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
	 * 	( y(i+1) - y(i)   m(i)	     m(i+1) - m(i)	)		 m(i+1) - m(i)		 ( y(i+2) - y(i+1)   m(i+1)	     m(i+2 - m(i+1)       )
	 * 	( ------------- - --- h(i) - ------------- h(i) ) + m(i)h(i) + 3 ------------- h(i)**2 = ( --------------- - ------ h(i+1) - ------------- h(i+1) )
	 * 	(      h(i)	   2	           6		)		     6 h(i)		 (      h(i+1)	        2	            6	          ) 
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
	 * 	[			...				...	  ] [  ...   ]     [
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
	
	// Calculate interval array 'h'
	splineH_.initialise(nPoints);
	for (i=0; i<nPoints-1; ++i) splineH_[i] = x_[i+1] - x_[i];

	// Initialise parameter arrays and working array
	splineA_.initialise(nPoints-1);
	splineB_.initialise(nPoints-1);
	splineC_.initialise(nPoints-1);
	splineD_.initialise(nPoints-1);

	constrainedSpline_ = constrained;
	
	if (!constrainedSpline_)
	{
		/*
		 * Unconstrained Spline Fit
		 */
		// Determine 'm' values (== C) now by solving the tridiagonal matrix above. Use Thomas algorithm...
		// -- First stage
		double p, q, r, s;
		Array<double> rprime(nPoints), sprime(nPoints);
		rprime[0] = 0.0;	// Would be 0.0 / 1.0 in the case of Natural spline
		sprime[0] = 0.0;

		for (i=1; i<nPoints-1; ++i)
		{
			// For a given i, p(i) = h(i-1), q(i) = 2(h(i-1)+h(i)), r(i) = h(i), s(i) = 6 ((y(n+1) - y(n)) / h(n) - (y(n) - y(n-1)) / h(n-1)
			p = splineH_[i-1];
			q = 2.0*(splineH_[i-1]+splineH_[i]);
			r = splineH_[i];
			s = 6.0 * ((y_[i+1] - y_[i]) / splineH_[i] - (y_[i] - y_[i-1]) / splineH_[i-1]);

			// -- Calculate r'(i) = r(i) / ( q(i) - r'(i-1)p(i) )
			rprime[i] = r / (q - rprime[i-1]*p);
			// -- Calculate s'(i) = (s(i) - s'(i-1)p(i) ) / ( q(i) - r'(i-1)p(i))
			sprime[i] = (s - sprime[i-1]*p) / (q - rprime[i-1]*p);
		}
		rprime[nPoints-1] = 0.0;
		sprime[nPoints-1] = (0.0 - sprime[nPoints-2]/splineH_[i-1]) / (2.0*splineH_[i-1]);

		// -- Second stage - backsubstitution
		splineC_[nPoints-1] = 0.0;
		for (i=nPoints-2; i>=0; --i)
		{
			// For a given i, m(i) = s'(i) - r'(i)m(i+1)
			splineC_[i] = sprime[i] - rprime[i]*splineC_[i+1];
		}
		
		// splineC_ array now contains m(i)...
		for (i=0; i<nPoints-1; ++i)
		{
			splineB_[i] = (y_[i+1] - y_[i]) / splineH_[i] - 0.5 * splineH_[i] * splineC_[i] - (splineH_[i] * (splineC_[i+1]-splineC_[i]))/6.0;
			splineD_[i] = (splineC_[i+1] - splineC_[i]) / (6.0 * splineH_[i]);
			splineC_[i] *= 0.5;
			splineA_[i] = y_[i];
		}
	}
	else
	{
		/*
		 * Constrained Spline fit
		 */
		
		// Calculate first derivatives at each point
		Array<double> fp(nPoints);
		double gradA, gradB;
		for (i=1; i<nPoints-1; ++i)
		{
			gradA = (x_[i+1] - x_[i])/(y_[i+1] - y_[i]);
			gradB = (x_[i] - x_[i-1])/(y_[i] - y_[i-1]);
			if (DUQMath::sgn(gradA) != DUQMath::sgn(gradB)) fp[i] = 0.0;
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
			splineD_[i-1] = (fppi - fppim1) / (6.0*dx);
			splineC_[i-1] = (x_[i]*fppim1 - x_[i-1]*fppi) / (2.0*dx);
			splineB_[i-1] = (dy - splineC_[i-1]*(x_[i]*x_[i] - x_[i-1]*x_[i-1]) - splineD_[i-1]*(x_[i]*x_[i]*x_[i] - x_[i-1]*x_[i-1]*x_[i-1])) / dx;
			splineA_[i-1] = y_[i-1] - splineB_[i-1]*x_[i-1] - splineC_[i-1]*x_[i-1]*x_[i-1] - splineD_[i-1]*x_[i-1]*x_[i-1]*x_[i-1];
		}
	}

	splineInterval_ = 0;
}

// Return spline interpolated y value for supplied x
double XYData::interpolated(double xvalue)
{
	// Do we need to (re)generate the interpolation?
	if (splineInterval_ == -1) interpolate();
	
// 	// X and ddy arrays match?
// 	if (x_.nItems() != splineB_.nItems())
// 	{
// 		Messenger::error("X and spline coefficient array sizes do not match (%i vs %i) in XYData::interpolated().\n", x_.nItems(), splineB_.nItems());
// 		return 0.0;
// 	}

	// Bracket x-value - we'll assume that access is likely to be more sequential than random...

	// Check old interval (and next one up...)
	if (xvalue < x_[splineInterval_])
	{
		if (splineInterval_ > 0)
		{
			--splineInterval_;
			if ((xvalue < x_[splineInterval_]) || (xvalue > x_[splineInterval_+1])) splineInterval_ = -1;
		}
		else return y_.first();
	}
	else if (xvalue > x_[splineInterval_+1])
	{
		if (splineInterval_ < splineB_.nItems()-2)
		{
			++splineInterval_;
			if ((xvalue < x_[splineInterval_]) || (xvalue > x_[splineInterval_+1])) splineInterval_ = -1;
		}
		else return y_.last();
	}

	// If interval is now (or still) -1, must search data...
	if (splineInterval_ == -1)
	{
		splineInterval_ = 0;
		int i, right = splineB_.nItems()-1;
		while ((right-splineInterval_) > 1)
		{
			i = (right+splineInterval_) / 2;
			if (x_[i] > xvalue) right = i;
			else splineInterval_ = i;
		}
	}

	return interpolated(xvalue, splineInterval_);
}

// Return spline interpolated y value for supplied x, specifying containing interval
double XYData::interpolated(double xvalue, int interval)
{
	// Calculate cubic polynomial
// 	double result = a*y_[splineBracketLeft_] + b*y_[splineBracketRight_] + ((a*a*a-a)*ddy_[splineBracketLeft_] + (b*b*b-b)*ddy_[splineBracketRight_])*(interval*interval)/6.0;
	if (interval < 0) return y_.first();
	else if (interval >= splineA_.nItems()) return y_.last();
	double h = constrainedSpline_ ? xvalue : xvalue - x_[interval];
	double hh = h*h;
	return splineA_[interval] + splineB_[interval]*h + splineC_[interval]*hh + splineD_[interval]*hh*h;
}

// Smooth data
void XYData::smooth(int avgSize, int skip)
{
	// First, create a new dataset using Y-averages of the current data
	XYData avg;
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

// Add interpolated data
void XYData::addInterpolated(XYData& source, double weighting)
{
	// If there is currently no data, just copy the source data
	if (x_.nItems() == 0)
	{
		x_ = source.arrayX();
		y_ = source.arrayY();
		y_ *= weighting;
	}
	else for (int n=0; n<x_.nItems(); ++n) addY(n, source.interpolated(x_.value(n)) * weighting);
}

/*
 * General Functions
 */

// Return minumum x value in data
double XYData::xMin()
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_[0];
	for (int n=1; n<x_.nItems(); ++n) if (x_[n] < result) result = x_[n];
	return result;
}

// Return maxumum x value in data
double XYData::xMax()
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_[0];
	for (int n=1; n<x_.nItems(); ++n) if (x_[n] > result) result = x_[n];
	return result;
}

// Return minumum y value in data
double XYData::yMin()
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_[0];
	for (int n=1; n<y_.nItems(); ++n) if (y_[n] < result) result = y_[n];
	return result;
}

// Return maxumum y value in data
double XYData::yMax()
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_[0];
	for (int n=1; n<y_.nItems(); ++n) if (y_[n] > result) result = y_[n];
	return result;
}

// Compute integral of the data
double XYData::integral()
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

// Compute absolute integral of the data
double XYData::absIntegral()
{
	if (nPoints() < 2) return 0.0;
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

// Apply median filter to data
void XYData::medianFilter(int length)
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

// Convolute this data with the supplied data, by products
bool XYData::convoluteProduct(XYData& data)
{
	// Check compatibility of datasets
	if (data.nPoints() != nPoints())
	{
		Messenger::error("Refusing to convolute by product two datasets of different sizes.\n");
		return false;
	}
	for (int n=0; n<nPoints(); ++n)
	{
		if (fabs(data.x(n) - x_[n]) > 1.0e-5)
		{
			Messenger::error("Refusing to convolute by product two datasets with different x-values.\n");
			return false;
		}
	}

	// Ready to go...
	for (int n=0; n<nPoints(); ++n) y_[n] *= data.y(n);
	
	return true;
}

// Trim data to X-range specified
void XYData::trim(double minX, double maxX)
{
	// Copy old data first...
	Array<double> oldX = x_;
	Array<double> oldY = y_;
	x_.forgetData();
	y_.forgetData();
	for (int n=0; n<oldX.nItems(); ++n)
	{
		if (oldX[n] < minX) continue;
		if (oldX[n] > maxX) break;
		addPoint(oldX[n], oldY[n]);
	}
}

// Rebin data onto uniform x axis
void XYData::rebin(double deltaX)
{
	// If deltaX is negative, work out a deltaX to use
	if (deltaX < 0.0)
	{
		deltaX = 0.0;
		for (int n=1; n<x_.nItems(); ++n) deltaX += x_[n]-x_[n-1];
		deltaX /= x_.nItems()-1;
		deltaX *= 0.5;
	}

	// Interpolate the existing data
	interpolate(true);

	// Generate new data
	XYData rebinnedData;
	double x = 0.0, xLimit = xMax();
	while (x < xLimit)
	{
		rebinnedData.addPoint(x, interpolated(x));
		x += deltaX;
	}

	// Overwrite old data
	(*this) = rebinnedData;
}
	
/*
 * File I/O
 */

// Load data from specified file
bool XYData::load(const char* filename)
{
	// Open file and check that we're OK to proceed reading from it
	LineParser parser;

	if ((!parser.openInput(filename)) || (!parser.isFileGoodForReading()))
	{
		Messenger::error("Couldn't open file '%s' for reading.\n", filename);
		return false;
	}

	int success, nCols = -1;
	clear();
	while (!parser.eofOrBlank())
	{
		success = parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks);
		if (success != 0)
		{
			parser.closeFiles();
			Messenger::error("Error reading from file '%s'.\n", filename);
			return false;
		}

		addPoint(parser.argd(0), parser.argd(1));
	}
	
	parser.closeFiles();
	
	if (nCols == 3) Messenger::print("Loaded %i points from file '%s' (including spline coefficients).\n", nPoints(), filename);
	else Messenger::print("Loaded %i points from file '%s'.\n", nPoints(), filename);
	
	return true;
}

// Save data to specified file
bool XYData::save(const char* filename) const
{
	// Open file and check that we're OK to proceed writing to it
	LineParser parser;

	parser.openOutput(filename, true);
	if (!parser.isFileGoodForWriting())
	{
		Messenger::error("Couldn't open file '%s' for writing.\n", filename);
		return false;
	}
	
	for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e\n", x_.value(n), y_.value(n));
	parser.closeFiles();
	return true;
}

// Save data and interpolation to specified file
bool XYData::saveWithInterpolation(const char* filename)
{
	// Open file and check that we're OK to proceed writing to it
	LineParser parser;
// 	Messenger::print("Writing datafile '%s'...\n", filename);

	parser.openOutput(filename, true);
	if (!parser.isFileGoodForWriting())
	{
		Messenger::error("Couldn't open file '%s' for writing.\n", filename);
		return false;
	}
	
	for (int n=0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e\n", x_.value(n), y_.value(n), interpolated(x_.value(n)));
	parser.closeFiles();
	return true;
}

// Dump contents
void XYData::dump()
{
	for (int n=0; n<x_.nItems(); ++n) Messenger::print("%16.10e  %16.10e  %16.10e\n", x_.value(n), y_.value(n), interpolated(x_.value(n)));
}

/*
 * Parallel Comms
 */

// Broadcast data
bool XYData::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// XY data
	if (!procPool.broadcast(x_, rootRank)) return false;
	if (!procPool.broadcast(y_, rootRank)) return false;

	// Spline data
	if (!procPool.broadcast(splineA_, rootRank)) return false;
	if (!procPool.broadcast(splineB_, rootRank)) return false;
	if (!procPool.broadcast(splineC_, rootRank)) return false;
	if (!procPool.broadcast(splineD_, rootRank)) return false;
	if (!procPool.broadcast(splineInterval_, rootRank)) return false;

	// Axis/title information
	if (!procPool.broadcast(name_, rootRank)) return false;
#endif
	return true;
}

// Load data from specified file (master) and distribute
bool XYData::load(const char* filename, ProcessPool& procPool)
{
	// Master will load the data
	if (procPool.isMaster())
	{
		if (!load(filename))
		{
			procPool.stop();
			return false;
		}
		procPool.proceed();
		if (!broadcast(procPool)) return false;
	}
	else
	{
		// Slaves first wait to see if the master loaded the file successfully
		if (!procPool.decision()) return false;
		if (!broadcast(procPool)) return false;
	}

	return true;
}
	
