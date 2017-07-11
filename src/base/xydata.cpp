/*
	*** XY Data
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

// Constructor
XYData::XYData() : ListItem<XYData>()
{
	name_ = "Untitled";
	interpolationInterval_ = -1;
	interpolationScheme_ = NoInterpolation;
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
	interpolationA_ = source.interpolationA_;
	interpolationB_ = source.interpolationB_;
	interpolationC_ = source.interpolationC_;
	interpolationD_ = source.interpolationD_;
	interpolationH_ = source.interpolationH_;
	interpolationInterval_ = source.interpolationInterval_;
	interpolationScheme_ = source.interpolationScheme_;
	name_ = source.name_;
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
	interpolationInterval_ = -1;
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
	interpolationA_ = source.interpolationA_;
	interpolationB_ = source.interpolationB_;
	interpolationC_ = source.interpolationC_;
	interpolationD_ = source.interpolationD_;
	interpolationH_ = source.interpolationH_;
	interpolationInterval_ = source.interpolationInterval_;
	interpolationScheme_ = source.interpolationScheme_;
}

// Copy existing X data and generate empty Y
void XYData::templateFrom(XYData& source)
{
	x_ = source.x_;
	y_.initialise(x_.nItems(), 0.0);
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

	avg.interpolate(XYData::ConstrainedSplineInterpolation);

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
	interpolate(XYData::ConstrainedSplineInterpolation);

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
			Messenger::error("Error reading from '%s', as one or both columns specified (%i and %i) are not present.\n", parser.inputFilename(), xcol, ycol);
			return false;
		}

		addPoint(parser.argd(0), parser.argd(1));
	}
	
	Messenger::print("Loaded %i points from '%s'.\n", nPoints(), parser.inputFilename());
	
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
