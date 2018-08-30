/*
	*** Fourier Transforms
	*** src/math/ft.cpp
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

#include "math/ft.h"
#include "math/xydata.h"
#include "base/sysfunc.h"

// Make some checks before doing transform
bool Fourier::checkBeforeTransform(XYData& data)
{
	// Enough data to do transform?
	if (data.nPoints() < 5)
	{
		Messenger::error("Not enough X data points (%i) in XYData. Can't do transform.\n", data.nPoints());
		return false;
	}

	return true;
}

// Perform Fourier sine transform of current distribution function, over range specified, and with specified broadening function, modification function, and window applied (if requested)
bool Fourier::sineFT(XYData& data, double normFactor, double wMin, double wStep, double wMax, WindowFunction windowFunction, BroadeningFunction broadening)
{
	/*
	 * Perform sine Fourier transform of current data. Function has no notion of forward or backwards transforms - normalisation and broadening functions must
	 * be suitable for the required purpose. Broadening functions are applied to the transformed function utilising convolution theorem:
	 * 
	 * 	f(x) and g(x) are the original functions, while F(q) and G(q) are their Fourier transforms.
	 * 	Pointwise multiplication (.) in one domain equals convolution (*) in the other:
	 * 
	 * 	FT[ f(x) * g(x) ] = F(q) . G(q)
	 * 	FT[ f(x) . g(x) ] = F(q) * G(q)
	 * 
	 * Since the ultimate goal of this function is to generate the broadened FT of the input data (with the broadening applied to the transformed data, rather than
	 * applied to the input data and then transformed) we require the first case listed above. The quantity we want is the pointwise multiplication of the FT of the 
	 * input data with the broadening functions given, so we can simply perform the convolution of the input data with the *FT* of the broadening functions, and FT 
	 * the result.
	 */

	// Okay to continue with transform?
	if (!checkBeforeTransform(data)) return false;

	// Set up window function for the present data
	windowFunction.setUp(data);

	// Grab x and y arrays
	const Array<double>& x = data.constX();
	Array<double>& y = data.y();

	int m;
	const int nX = x.nItems();
	double window, broaden;

	// Create working array
	XYData sineft;

	// Assume deltaX is the difference between the first two points
	double deltaX = x.constAt(1) - x.constAt(0);

	// Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
	double ft;
	double omega = wMin;
	while (omega <= wMax)
	{
		ft = 0.0;
		for (m=0; m<nX-1; ++m)
		{
			deltaX = x.constAt(m+1) - x.constAt(m);

			// Get window value at this position in the function
			window = windowFunction.y(x.constAt(m), omega);

			// Calculate broadening
			broaden = broadening.yFT(x.constAt(m), omega);

			ft += sin(x.constAt(m)*omega) * x.constAt(m) * broaden * window * y[m] * deltaX;
		}

		// Normalise
		if (omega > 0.0) ft /= omega;
		sineft.addPoint(omega, ft);
		
		omega += wStep;
	}

	// Transfer working arrays to this object
	data.copyData(sineft);

	// Apply normalisation factor
	y *= normFactor;

	return true;
}

// // Fourier transform current data, applying line-width broadening in real-space using the modified Lorch function
// bool XYData::transformLorch(double atomicDensity, double step, double rMax, double beta, double delta0, bool qToR)
// {
// 	/*
// 	 * Fourier transforms from Q to r-space, or r to Q-space, employing the modified Lorch function as described by Soper in
// 	 * XXX TODO.
// 	 * XXX TODO Only valid when input x_[] values are bin boundaries, not centre-bin values.
// 	 */
// 
// 	// Okay to continue with transform?
// 	if (!checkBeforeTransform()) return false;
// 
// 	double deltaIn = x_[1] - x_[0];
// 	double deltar, width, r, factor, norm, ftx, qr, j1qr, j2qr;
// 	int n, m, nPts = x_.nItems();
// 
// 	// Create working arrays
// 	Array<double> result;
// 	r = step * 0.5;
// 
// 	// Set up correct factor, depending on whether we are going from Q -> r or r -> Q
// 	if (qToR) factor = 1.0 / (2.0 * PI * PI * atomicDensity);
// 	else factor = 4.0 * PI * atomicDensity;
// 	
// 	// Perform Fourier sine transform
// 	while (r <= rMax)
// 	{
// 		// Reset sum, and calculate r-dependent values
// 		ftx = 0.0;
// 		norm = factor / (r*r*r);
// 		deltar = delta0 * (1.0 + pow(r, beta));
// 
// 		// Calculate first argument at left-hand bin boundary
// 		qr = r * x_[0];
// 		j1qr = sin(qr) - qr * cos(qr);
// 
// 		for (m=0; m<nPts-1; ++m)
// 		{
// 			// Calculate second argument at right-hand bin boundary
// 			qr = r * x_[m+1];
// 			j2qr = sin(qr) - qr * cos(qr);
// 
// 			// Calculate 'width' at centre of bin
// 			width = 0.5 * (x_[m]+x_[m+1]) * deltar;
//             
// 			// Form integral of (Q sin Qr)/r from r1 to r2 (as detailed in equation 3.86 in reference above)
// 			// Modified Lorch function is constructed and applied if the width is non-zero and positive
// 			if (width < 0.0) ftx += y_[m] * (j2qr - j1qr);
// 			else ftx += y_[m] * (j2qr - j1qr) * (3.0/(width*width*width)) * (sin(width) - width*cos(width));
// 
// 			// Overwrite first Lorch argument
// 			j1qr = j2qr;
// 		}
// 
// 		// Sum
// 		result.add(ftx * norm);
// 		
// 		r += step;
// 	}
// 
// 	// Copy transform data over initial data
// 	y_ = result;
// 	x_.forgetData();
// 	for (n=0; n<y_.nItems(); ++n) x_.add((n+0.5)*step);
// 	interpolationInterval_ = -1;
// 
// 	return true;
// }
