/*
	*** XY Data - Fourier Transforms
	*** src/base/xydata_ft.cpp
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

#include "base/xydata.h"
#include "base/sysfunc.h"

// Static Members
bool XYData::useFFT_ = false;

// Window Function keywords
const char* WindowFunctionKeywords[] = { "None", "Bartlett", "Hann", "Lanczos", "Nuttall", "Sine" };

// Convert text string to WindowFunction
XYData::WindowFunction XYData::windowFunction(const char* s)
{
	for (int n=0; n<XYData::nWindowFunctions; ++n) if (DUQSys::sameString(s,WindowFunctionKeywords[n])) return (XYData::WindowFunction) n;
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
	interpolationInterval_ = -1;
	return true;
}

// Perform Fourier sine transform of current distribution function, over range specified, and with specified broadening functions and window applied (if requested)
bool XYData::sineFT(double normFactor, double wMin, double wStep, double wMax, const Function& generalBroadening, const Function& wDependentBroadening, XYData::WindowFunction wf)
{
	// Okay to continue with transform?
	if (!checkBeforeTransform()) return false;

	int n, m;
	const int nX = x_.nItems();
	double windowPos, broadening;

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
// 		for (n=0; n<x_.nItems(); ++n) x_[n] = (n+0.5)*k;  k not defined anymore
		for (n=0; n<x_.nItems(); ++n) y_[n] = yin[n];
		y_ *= 2.0 / x_.nItems();
		y_[0] / 2.0;
	}
	else
	{
		// Create working array
		XYData sineft;

		// Assume deltaX is the difference between the first two points
		double deltaX = x_[1] - x_[0];

		// Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
		double ft;
		double omega = wMin;
		while (omega <= wMax)
		{
			ft = 0.0;
			for (m=0; m<nX; ++m)
			{
				// Get window value at this position in the function
				windowPos = double(m) / double(nX-1);

				// Calculate broadening
				broadening = generalBroadening.value(x_[m]) * wDependentBroadening.value(x_[m], omega);

				ft += sin(x_[m]*omega) * x_[m] * broadening * window(wf, windowPos) * y_[m] * deltaX;
			}

			// Normalise
			if (omega > 0.0) ft /= omega;
			sineft.addPoint(omega, ft);
			
			omega += wStep;
		}

		// Transfer working arrays to this object
		copyData(sineft);
	}

	// Apply normalisation factor
	y_ *= normFactor;

	interpolationInterval_ = -1;

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
	
	// Perform Fourier sine transform
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
	interpolationInterval_ = -1;

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
	interpolationInterval_ = -1;
	return true;
}

