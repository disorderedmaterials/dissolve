/*
	*** XY Data
	*** src/math/xydata.cpp
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

#include "base/processpool.h"
#include "math/integrator.h"
#include "math/xydata.h"
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
	xColumn_ = -1;
	yColumn_ = -1;
	z_ = 0.0;
}

// Destructor
XYData::~XYData()
{
}

// Copy Constructor
XYData::XYData(const XYData& source) : ObjectStore<XYData>(this)
{
	(*this) = source;
}

// Clear Data
void XYData::clear()
{
	x_.clear();
	y_.clear();
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
}

// Initialise arrays to specified size
void XYData::initialise(int size)
{
	clear();
	x_.initialise(size);
	y_.initialise(size);
	z_ = 0.0;
}

// Copy existing X and Y data
void XYData::copyData(const XYData& source)
{
	x_ = source.x_;
	y_ = source.y_;
	z_ = source.z_;
}

// Copy existing X data and generate empty Y
void XYData::templateFrom(const XYData& source)
{
	x_ = source.x_;
	y_.initialise(x_.nItems());
	y_ = 0.0;
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
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::setX().\n", index);
		return;
	}
#endif
	x_[index] = x;
}

// Return x value specified
double& XYData::x(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		static double dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::x().\n", index);
		return dummy;
	}
#endif
	return x_[index];
}

// Return x value specified (const)
double XYData::constX(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in XYData::constX().\n", index);
		return 0.0;
	}
#endif
	return x_.constAt(index);
}

// Return x Array
Array<double>& XYData::arrayX()
{
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
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::setY().\n", index);
		return;
	}
#endif
	y_[index] = y;
}

// Add to y value
void XYData::addY(int index, double delta)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::addY().\n", index);
		return;
	}
#endif
	y_[index] += delta;
}

// Add to y array
bool XYData::addY(const Array<double>& source, double factor)
{
	if (y_.nItems() != source.nItems())
	{
		Messenger::error("BAD_USAGE - Can't add Y values from source array, since the number of items differs (%i vs %i).\n", y_.nItems(), source.nItems());
		return false;
	}

	for (int n=0; n<y_.nItems(); ++n) y_[n] += source.constAt(n)*factor;
	return true;
}

// Multiply y value
void XYData::multiplyY(int index, double factor)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::multiplyY().\n", index);
		return;
	}
#endif
	y_[index] *= factor;
}

// Return y value specified
double& XYData::y(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		static double dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::y().\n", index);
		return dummy;
	}
#endif
	return y_[index];
}

// Return y value specified (const)
double XYData::constY(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in XYData::constY().\n", index);
		return 0.0;
	}
#endif
	return y_.constAt(index);
}

// Return y Array
Array<double>& XYData::arrayY()
{
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
		if (fabs(x_.constAt(n) - source.x_.constAt(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't + these XYData, since the x axes are different.\n");
			return newData;
		}
		newData.y_[n] += source.y_.constAt(n);
	}
	
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
		if (fabs(x_.constAt(n) - source.x_.constAt(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't += these XYData, since the x axes are different.\n");
			return;
		}
		y_[n] += source.y_.constAt(n);
	}
}

// Operator +=
void XYData::operator+=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] += dy;
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
		if (fabs(x_.constAt(n) - source.x_.constAt(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't - these XYData, since the x axes are different.\n");
			return newData;
		}
		newData.y_[n] -= source.y_.constAt(n);
	}
	
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
		if (fabs(x_.constAt(n) - source.x_.constAt(n)) > OPTOLERANCE)
		{
			Messenger::error("Can't -= these XYData, since the x axes are different.\n");
			return;
		}
		y_[n] -= source.y_.constAt(n);
	}
}

// Operator -=
void XYData::operator-=(const double dy)
{
	for (int n=0; n<x_.nItems(); ++n) y_[n] -= dy;
}

// Operator *=
void XYData::operator*=(const double factor)
{
	// Multiply current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] *= factor;
}

// Operator /=
void XYData::operator/=(const double factor)
{
	// Divide current data
	for (int n=0; n<x_.nItems(); ++n) y_[n] /= factor;
}

/*
 * Limits
 */

// Return minimum (first) x value in data
double XYData::xMin() const
{
	return x_.firstValue();
}

// Return maximum (last) x value in data
double XYData::xMax() const
{
	return x_.lastValue();
}

// Return minumum y value in data
double XYData::yMin() const
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_.constAt(0);
	for (int n=1; n<y_.nItems(); ++n) if (y_.constAt(n) < result) result = y_.constAt(n);
	return result;
}

// Return maxumum y value in data
double XYData::yMax() const
{
	if (y_.nItems() == 0) return 0.0;
	double result = y_.constAt(0);
	for (int n=1; n<y_.nItems(); ++n) if (y_.constAt(n) > result) result = y_.constAt(n);
	return result;
}

/*
 * Gradients
 */

// Return gradient of last n points, and average y value if requested
double XYData::lastGradient(int nSamples, double* yMean) const
{
	// Work out standard deviation of energy points
	double Sx = 0.0, Sy = 0.0, Sxy = 0.0;
	double xBar = 0.0, yBar = 0.0;

	// Calculate mean values
	for (int n=nPoints()-nSamples; n<nPoints(); ++n)
	{
		xBar += n;
		yBar += y_.constAt(n);
	}
	xBar /= nSamples;
	yBar /= nSamples;

	// Determine Sx, Sy, and Sxy
	for (int n=nPoints()-nSamples; n<nPoints(); ++n)
	{
		Sx += (n - xBar)*(n - xBar);
		Sy += (y_.constAt(n) - yBar)*(y_.constAt(n) - yBar);
		Sxy += (n - xBar) * (y_.constAt(n) - yBar);
	}

	// Set yMean value (if a variable was provided)
	if (yMean) (*yMean) = yBar;

	// Return the gradient
	return Sxy / Sx;
}

/*
 * Filtering
 */

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
}

// Apply Kolmogorov–Zurbenko filter
void XYData::kolmogorovZurbenkoFilter(int k, int m)
{
	for (int iteration=0; iteration<k; ++iteration) movingAverage(m);
}

/*
 * Manipulation
 */

// Perform point-wise convolution of this data with the supplied BroadeningFunction
void XYData::convolve(BroadeningFunction function)
{
	Array<double> newY(y_.nItems());

	// Outer loop over existing data points
	double xCentre, xBroad;
	for (int n=0; n<x_.nItems(); ++n)
	{
		// Grab current x value as our current xCentre
		xCentre = x_[n];

		// Inner loop over new values array
		for (int m=0; m<x_.nItems(); ++m)
		{
			xBroad = x_[m] - xCentre;
			newY[m] += y_[n] * function.y(xBroad, 0.0);
		}
	}

	y_ = newY;
}

// Perform point-wise convolution of this data with the supplied BroadeningFunction, normalising to the original integral of the function
void XYData::convolveNormalised(BroadeningFunction function)
{
	// Calculate the original integral
	double originalIntegral = Integrator::absIntegral(*this);

	// If the original integral is zero, nothing to do
	if (originalIntegral == 0.0) return;

	// Convolve the function
	convolve(function);

	// Calculate the new integral
	double newIntegral = Integrator::absIntegral(*this);

	y_ *= (originalIntegral / newIntegral);
}

// Subtract average level from data, forming average from supplied x value
double XYData::subtractAverage(double xStart)
{
	double sum = 0.0;
	int nPoints = 0;
	for (int n=0; n<x_.nItems(); ++n)
	{
		if (x(n) >= xStart)
		{
			sum += y(n);
			++nPoints;
		}
	}
	y_ -= sum / nPoints;

	return sum / nPoints;
}

/*
 * File I/O
 */

// Return filename from which the data was loaded (if any)
const char* XYData::sourceFilename() const
{
	return sourceFilename_.get();
}

// Return column used for X values when read from file (if relevant)
int XYData::xColumn() const
{
	return xColumn_;
}

// Return column used for Y values when read from file (if relevant)
int XYData::yColumn() const
{
	return yColumn_;
}

// Load data from specified LineParser, using columns specified
bool XYData::load(LineParser& parser, int xcol, int ycol)
{
	int success;
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

	Messenger::printVerbose("Read %i points from '%s' (columns %i and %i).\n", nPoints(), parser.inputFilename(), xcol+1, ycol+1);

	return true;
}

// Load data from specified file
bool XYData::load(const char* filename, int xcol, int ycol)
{
	// Open file and check that we're OK to proceed reading from it
	LineParser parser;

	if ((!parser.openInput(filename)) || (!parser.isFileGoodForReading()))
	{
		Messenger::error("Couldn't open file '%s' for reading.\n", filename);
		return false;
	}

	bool result = load(parser, xcol, ycol);

	parser.closeFiles();

	sourceFilename_ = filename;
	xColumn_ = xcol;
	yColumn_ = ycol;

	return result;
}

// Load data from specified file through ProcessPool, using columns specified
bool XYData::load(ProcessPool& pool, const char* filename, int xcol, int ycol)
{
	// Open file across ProcessPool and check that we're OK to proceed reading from it
	LineParser parser(&pool);

	if ((!parser.openInput(filename)) || (!parser.isFileGoodForReading()))
	{
		Messenger::error("Couldn't open file '%s' for reading.\n", filename);
		return false;
	}

	bool result = load(parser, xcol, ycol);

	parser.closeFiles();

	sourceFilename_ = filename;
	xColumn_ = xcol;
	yColumn_ = ycol;

	return result;
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
	
	for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e\n", x_.constAt(n), y_.constAt(n));
	parser.closeFiles();

	return true;
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
	if (sourceFilename_.isEmpty())
	{
		if (!parser.writeLineF("'%s'\n", name_.get())) return false;
	}
	else if (!parser.writeLineF("'%s'  '%s'  %i  %i\n", name_.get(), sourceFilename_.get(), xColumn_, yColumn_)) return false;
	if (!parser.writeLineF("%s\n", objectName())) return false;
	if (!parser.writeLineF("%f\n", z_)) return false;
	if (!parser.writeLineF("%i\n", nPoints())) return false;
	for (int n=0; n<nPoints(); ++n) if (!parser.writeLineF("%16.9e %16.9e\n", x_.constAt(n), y_.constAt(n))) return false;
	return true;
}

// Read data through specified LineParser
bool XYData::read(LineParser& parser)
{
	clear();

	if (parser.getArgsDelim(LineParser::UseQuotes) != LineParser::Success) return false;
	name_ = parser.argc(0);
	if (parser.hasArg(1)) sourceFilename_ = parser.argc(1);
	else sourceFilename_.clear();
	xColumn_ = parser.hasArg(2) ? parser.argi(2) : -1;
	yColumn_ = parser.hasArg(3) ? parser.argi(3) : -1;

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
	if (!procPool.equality(nPoints())) return Messenger::error("XYData number of points is not equivalent (process %i has %i).\n", procPool.poolRank(), nPoints());
	for (int n=0; n<nPoints(); ++n)
	{
		if (!procPool.equality(x_[n])) return Messenger::error("XYData x value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), x_[n]);
		if (!procPool.equality(y_[n])) return Messenger::error("XYData y value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), y_[n]);
	}
	if (!procPool.equality(z_)) return Messenger::error("XYData z value is not equivalent (process %i has %e).\n", procPool.poolRank(), z_);
#endif
	return true;
}
