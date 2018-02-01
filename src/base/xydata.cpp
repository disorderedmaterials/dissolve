/*
	*** XY Data
	*** src/base/xydata.cpp
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

#include "base/processpool.h"
#include "base/xydata.h"
#include "base/messenger.h"
#include "base/lineparser.h"
#include "math/constants.h"
#include <math.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
using namespace std;

// Static Members
template<class XYData> RefList<XYData,int> ObjectStore<XYData>::objects_;
template<class XYData> int ObjectStore<XYData>::objectCount_ = 0;
template<class XYData> int ObjectStore<XYData>::objectType_ = ObjectInfo::XYDataObject;
template<class XYData> const char* ObjectStore<XYData>::objectTypeName_ = "XYData";

// Constructor
XYData::XYData() : ListItem<XYData>(), ObjectStore<XYData>(this) 
{
	name_ = "Untitled";
	interpolationInterval_ = -1;
	interpolationScheme_ = NoInterpolation;
	z_ = 0.0;
}

// Destructor
XYData::~XYData()
{
}

// Copy Constructor
XYData::XYData(const XYData& source) : ObjectStore<XYData>(NULL)
{
	(*this) = source;
}

// Clear Data
void XYData::clear()
{
	x_.clear();
	y_.clear();
	clearInterpolationArrays();
}

// Clear interpolation arrays
void XYData::clearInterpolationArrays()
{
	interpolationA_.clear();
	interpolationB_.clear();
	interpolationC_.clear();
	interpolationD_.clear();
	interpolationH_.clear();
	interpolationInterval_ = -1;
	interpolationScheme_ = NoInterpolation;
}

/*
 * Data
 */

// Reset arrays to zero
void XYData::reset()
{
	for (int n=0; n<x_.nItems(); ++n) x_[n] = 0.0;
	for (int n=0; n<y_.nItems(); ++n) y_[n] = 0.0;
	z_ = 0.0;
	interpolationInterval_ = -1;
}

// Initialise arrays to specified size
void XYData::initialise(int size)
{
	clear();
	x_.initialise(size);
	y_.initialise(size);
	z_ = 0.0;
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
	z_ = source.z_;
	interpolationA_ = source.interpolationA_;
	interpolationB_ = source.interpolationB_;
	interpolationC_ = source.interpolationC_;
	interpolationD_ = source.interpolationD_;
	interpolationH_ = source.interpolationH_;
	interpolationInterval_ = source.interpolationInterval_;
	interpolationScheme_ = source.interpolationScheme_;
}

// Copy existing X data and generate empty Y
void XYData::templateFrom(const XYData& source)
{
	x_ = source.x_;
	y_.initialise(x_.nItems());
	y_ = 0.0;
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
	return x_;
}

// Return x Array (const)
const Array<double>& XYData::constArrayX() const
{
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
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
	interpolationInterval_ = -1;
	return y_;
}

// Return y Array (const)
const Array<double>& XYData::constArrayY() const
{
	return y_;
}

// Set z value
void XYData::setZ(double z)
{
	z_ = z;
}

// Add to z value
void XYData::addZ(double delta)
{
	z_ += delta;
}

// Return z value
double& XYData::z()
{
	return z_;
}

// Return z value (const)
double XYData::constZ() const
{
	return z_;
}

// Add new data point
void XYData::addPoint(double x, double y)
{
	x_.add(x);
	y_.add(y);
	interpolationInterval_ = -1;
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
	z_ = source.z_;
	interpolationA_ = source.interpolationA_;
	interpolationB_ = source.interpolationB_;
	interpolationC_ = source.interpolationC_;
	interpolationD_ = source.interpolationD_;
	interpolationH_ = source.interpolationH_;
	interpolationInterval_ = source.interpolationInterval_;
	interpolationScheme_ = source.interpolationScheme_;
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
	
	newData.interpolationInterval_ = -1;
	
	return newData;
}

// Operator +=
void XYData::operator+=(const XYData& source)
{
	// If source array is empty, nothing to do
	if (source.nPoints() == 0) return;

	// Initialise current arrays?
	if (x_.nItems() == 0) templateFrom(source);

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

	interpolationInterval_ = -1;
}

// Operator +=
void XYData::operator+=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] += dy;
	interpolationInterval_ = -1;
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
	
	newData.interpolationInterval_ = -1;
	
	return newData;
}

// Operator -=
void XYData::operator-=(const XYData& source)
{
	// Initialise current arrays?
	if (x_.nItems() == 0) templateFrom(source);

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

	interpolationInterval_ = -1;
}

// Operator -=
void XYData::operator-=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] -= dy;
	interpolationInterval_ = -1;
}

// Operator *=
void XYData::operator*=(const double factor)
{
	// Multiply current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] *= factor;
	interpolationInterval_ = -1;
}

// Operator /=
void XYData::operator/=(const double factor)
{
	// Divide current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] /= factor;
	interpolationInterval_ = -1;
}

/*
 * General Functions
 */

// Return minumum x value in data
double XYData::xMin() const
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_.value(0);
	for (int n=1; n<x_.nItems(); ++n) if (x_.value(n) < result) result = x_.value(n);
	return result;
}

// Return maxumum x value in data
double XYData::xMax() const
{
	if (x_.nItems() == 0) return 0.0;
	double result = x_.value(0);
	for (int n=1; n<x_.nItems(); ++n) if (x_.value(n) > result) result = x_.value(n);
	return result;
}

// Return first x value in data
double XYData::xFirst() const
{
	return x_.value(0);
}

// Return last x value in data
double XYData::xLast() const
{
	return x_.value(x_.nItems()-1);
}

// Return minumum y value in data
double XYData::yMin() const
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_.value(0);
	for (int n=1; n<y_.nItems(); ++n) if (y_.value(n) < result) result = y_.value(n);
	return result;
}

// Return maxumum y value in data
double XYData::yMax() const
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_.value(0);
	for (int n=1; n<y_.nItems(); ++n) if (y_.value(n) > result) result = y_.value(n);
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

// Convolute this data with the supplied data
bool XYData::convolute(XYData& data)
{
	Messenger::error("Point-wise convolution of XYData sets is not yet implemented.\n");
	return false;
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
	interpolate(XYData::LinearInterpolation);

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

// Perform moving average smoothing
void XYData::movingAverage(int avgSize)
{
	// Make sure avgSize is odd
	if (avgSize%2 == 0) --avgSize;

	Array<double> newY(x_.nItems());
	newY = 0.0;
	int n, m, i = avgSize/2;

	// Left-most region of data
	for (n=0; n<i; ++n)
	{
		for (m=0; m<=n+i; ++m) newY[n] += y_[m];
		newY[n] /= (i + 1 + n);
	}

	// Central region (full average width available)
	for (n=i; n < x_.nItems()-i; ++n)
	{
		for (m=n-i; m <= n+i; ++m) newY[n] += y_[m];
		newY[n] /= avgSize;
	}

	// Right-most region of data
	for (n=x_.nItems()-i; n<x_.nItems(); ++n)
	{
		for (m=n-i; m<x_.nItems(); ++m) newY[n] += y_[m];
		newY[n] /= (x_.nItems() - n + i + 1);
	}

	y_ = newY;

	interpolationInterval_ = -1;
}

// Apply Kolmogorov–Zurbenko filter
void XYData::kolmogorovZurbenkoFilter(int k, int m)
{
	for (int iteration=0; iteration<k; ++iteration) movingAverage(m);
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

// Return RMSE of current data with (interpolated) reference data
double XYData::rmse(XYData ref)
{
	// First, generate interpolation of reference data if it needs it
	if (ref.interpolationScheme_ == XYData::NoInterpolation) ref.interpolate(XYData::SplineInterpolation);

	// Generate RMSE over actual values of our data
	double rmse = 0.0, delta;
	double firstX = 0.0, lastX = 0.0;
	int nPointsConsidered = 0;
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Is our x value lower than the lowest x value of the reference data?
		if (x_[n] < ref.xFirst()) continue;

		// Is our x value higher than the last x value of the reference data?
		if (x_[n] > ref.xLast()) break;

		// Is this the first point considered?
		if (nPointsConsidered == 0) firstX = x_[n];

		// Sum squared error
		delta = y_[n] - ref.interpolated(x_[n]);
		rmse += delta*delta;
		lastX = x_[n];
		++nPointsConsidered;
	}

	// Finalise RMSE and summarise result
	rmse = sqrt(rmse/nPointsConsidered);
	Messenger::print("RMSE between datasets is %15.9e over %15.9e < x < %15.9e (%i points).\n", rmse, firstX, lastX, nPointsConsidered);

	return rmse;
}

// Return percentage error between this and reference data
double XYData::error(XYData ref)
{
	// First, generate interpolation of reference data if it needs it
	if (ref.interpolationScheme_ == XYData::NoInterpolation) ref.interpolate(XYData::SplineInterpolation);

	/*
	 * Generate error estimate over actual values of our own data
	 * We will calculate the mean absolute percentage error (MAPE) as well as the simple sum of errors divided by sum of reference data.
	 * We do both, and return the higher of the two, since the MAPE is not a good measure when considering very spiky data (since the
	 * on-step forecast is often worse than the actual error of the predicted value.
	 */
	
	double sume = 0.0, sumf = 0.0, sumy = 0.0;
	double firstX = 0.0, lastX = 0.0;
	int nPointsConsidered = 0;
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Is our x value lower than the lowest x value of the reference data?
		if (x_[n] < ref.xFirst()) continue;

		// Is our x value higher than the last x value of the reference data?
		if (x_[n] > ref.xLast()) break;

		// Is this the first point considered?
		if (nPointsConsidered == 0) firstX = x_[n];

		// Accumulate numerator - sum of forecast errors
		sume += fabs(y_[n] - ref.interpolated(x_[n]));
		sumy += fabs(ref.interpolated(x_[n]));

		// Accumulate denominator - one-step naive forecast (backwards forecast for first point)
		if (nPointsConsidered > 0) sumf += fabs(y_[n] - y_[n-1]);
		else if (n < x_.nItems()-1) sumf += fabs(y_[n] - y_[n+1]);

		lastX = x_[n];
		++nPointsConsidered;
	}

	// Finalise MAPE and summarise result
	double denominator;
	// Normalisation to N/(N-1) not performed when using one-step backwards forecast for first point.
// 	if (sumf > 0.0) denominator = (double(nPointsConsidered) / double(nPointsConsidered-1)) * sumf;
	if (sumf > 0.0) denominator = sumf;
	else denominator = 1.0;
	double mape = sume / denominator;
	Messenger::print("MAPE between datasets is %7.3f%% over %15.9e < x < %15.9e (%i points), d(|Y|)/sum(|Y|) = %7.3f%%.\n", mape, firstX, lastX, nPointsConsidered, sume/sumy);

	return max(mape,sume/sumy);
}

/*
 * File I/O
 */

// Load data from specified LineParser, using columns specified
bool XYData::load(LineParser& parser, int xcol, int ycol)
{
	int success, nCols = -1;
	clear();
	while (!parser.eofOrBlank())
	{
		success = parser.getArgsDelim(LineParser::Defaults+LineParser::SkipBlanks+LineParser::StripComments);
		if (success != 0)
		{
			parser.closeFiles();
			Messenger::error("Error reading from '%s'.\n", parser.inputFilename());
			return false;
		}

		if ((xcol >= parser.nArgs()) || (ycol >= parser.nArgs()))
		{
			Messenger::error("Error reading from '%s', as one or both columns specified (%i and %i) are not present.\n", parser.inputFilename(), xcol+1, ycol+1);
			return false;
		}

		addPoint(parser.argd(xcol), parser.argd(ycol));
	}
	
	Messenger::printVerbose("Loaded %i points from '%s' (columns %i and %i).\n", nPoints(), parser.inputFilename(), xcol+1, ycol+1);
	
	return true;
}

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
 * GenericItemBase Implementations
 */

// Return class name
const char* XYData::itemClassName()
{
	return "XYData";
}

// Write data through specified LineParser
bool XYData::write(LineParser& parser)
{
	if (!parser.writeLineF("%s\n", name_.get())) return false;
	if (!parser.writeLineF("%s\n", objectName())) return false;
	if (!parser.writeLineF("%f\n", z_)) return false;
	if (!parser.writeLineF("%i\n", nPoints())) return false;
	for (int n=0; n<nPoints(); ++n) if (!parser.writeLineF("%16.9e %16.9e\n", x_.value(n), y_.value(n))) return false;
	return true;
}

// Read data through specified LineParser
bool XYData::read(LineParser& parser)
{
	clear();
	if (parser.readNextLine(LineParser::Defaults, name_) != LineParser::Success) return false;
	if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) return false;
	setObjectName(parser.line());
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	z_ = parser.argd(0);
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int np = parser.argi(0);
	for (int n=0; n<np; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		addPoint(parser.argd(0), parser.argd(1));
	}
	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool XYData::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// XY(Z) data
	if (!procPool.broadcast(x_, rootRank)) return false;
	if (!procPool.broadcast(y_, rootRank)) return false;
	if (!procPool.broadcast(z_, rootRank)) return false;

	// Spline data
	if (!procPool.broadcast(interpolationA_, rootRank)) return false;
	if (!procPool.broadcast(interpolationB_, rootRank)) return false;
	if (!procPool.broadcast(interpolationC_, rootRank)) return false;
	if (!procPool.broadcast(interpolationD_, rootRank)) return false;
	if (!procPool.broadcast(interpolationH_, rootRank)) return false;
	if (!procPool.broadcast(interpolationInterval_, rootRank)) return false;

	// Axis/title information
	if (!procPool.broadcast(name_, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool XYData::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Check number of items in arrays first
	if (!procPool.equality(nPoints())) return Messenger::error("Number of points is not equivalent (process %i has %i).\n", procPool.poolRank(), nPoints());
	for (int n=0; n<nPoints(); +n)
	{
		if (!procPool.equality(x_[n])) return Messenger::error("X value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), x_[n]);
		if (!procPool.equality(y_[n])) return Messenger::error("Y value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), y_[n]);
	}
	if (!procPool.equality(z_)) return Messenger::error("Z value is not equivalent (process %i has %e).\n", procPool.poolRank(), z_);
#endif
	return true;
}
