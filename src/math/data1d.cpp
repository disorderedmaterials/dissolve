/*
	*** 1-Dimensional Data With Statistics
	*** src/math/data1d.cpp
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

#include "math/data1d.h"
#include "math/histogram1d.h"
#include "base/messenger.h"
#include "base/lineparser.h"

// Static Members (ObjectStore)
template<class Data1D> RefList<Data1D,int> ObjectStore<Data1D>::objects_;
template<class Data1D> int ObjectStore<Data1D>::objectCount_ = 0;
template<class Data1D> int ObjectStore<Data1D>::objectType_ = ObjectInfo::Data1DObject;
template<class Data1D> const char* ObjectStore<Data1D>::objectTypeName_ = "Data1D";

// Constructor
Data1D::Data1D() : ListItem<Data1D>(), ObjectStore<Data1D>(this) 
{
	clear();
}

// Destructor
Data1D::~Data1D()
{
}

// Copy Constructor
Data1D::Data1D(const Data1D& source) : ObjectStore<Data1D>(this)
{
	(*this) = source;
}

// Clear Data
void Data1D::clear()
{
	x_.clear();
	y_.clear();
}

/*
 * Data
 */

// Initialise to be consistent with supplied Histogram1D object
void Data1D::initialise(const Histogram1D& source)
{
	clear();

	x_ = source.binCentres();
	y_.initialise(x_.nItems());
}

// Zero values array
void Data1D::zero()
{
	y_ = SampledDouble();
}

// Accumulate specified histogram data
void Data1D::accumulate(const Histogram1D& source)
{
	// Check array sizes
	if (x_.nItems() == 0) initialise(source);
	else if (x_.nItems() != source.nBins())
	{
		Messenger::error("Can't add Histogram data - arrays are of different size.\n");
		return;
	}
	else
	{
		for (int n=0; n<y_.nItems(); ++n) y_[n] += source.constBin(n);
	}
}

// Add new data point
void Data1D::addPoint(double x, double y)
{
	x_.add(x);
	y_.add(y);
}


// Return x value specified
double& Data1D::x(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		static double dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in Data1D::x().\n", index);
		return dummy;
	}
#endif
	return x_[index];
}

// Return x value specified (const)
double Data1D::constX(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in Data1D::constX().\n", index);
		return 0.0;
	}
#endif
	return x_.constAt(index);
}

// Return x Array
Array<double>& Data1D::x()
{
	return x_;
}

// Return x Array (const)
const Array<double>& Data1D::constX() const
{
	return x_;
}

// Return y value specified
SampledDouble& Data1D::y(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		static SampledDouble dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in Data1D::y().\n", index);
		return dummy;
	}
#endif
	return y_[index];
}

// Return y value specified (const)
SampledDouble Data1D::constY(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= y_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for y_ array in Data1D::constY().\n", index);
		return 0.0;
	}
#endif
	return y_.constAt(index);
}

// Return y Array
Array<SampledDouble>& Data1D::y()
{
	return y_;
}

// Return y Array (const)
const Array<SampledDouble>& Data1D::constY() const
{
	return y_;
}

/*
 * Operators
 */

// Operator =
void Data1D::operator=(const Data1D& source)
{
	x_ = source.x_;
	y_ = source.y_;
}

/*
 * File I/O
 */

// Load data from specified LineParser, using columns specified
bool Data1D::load(LineParser& parser, int xcol, int ycol)
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

	Messenger::printVerbose("Read %i points from '%s' (columns %i and %i).\n", nDataPoints(), parser.inputFilename(), xcol+1, ycol+1);

	return true;
}

// Load data from specified file
bool Data1D::load(const char* filename, int xcol, int ycol)
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

	return result;
}

// Load data from specified file through ProcessPool, using columns specified
bool Data1D::load(ProcessPool& pool, const char* filename, int xcol, int ycol)
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

	return result;
}

// Save data to specified file
bool Data1D::save(const char* filename) const
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
 * Plottable Implementation
 */

// Return type of plottable
Plottable::PlottableType Data1D::plottableType() const
{
	return Plottable::OneAxisPlottable;
}

// Return number of points along x axis
int Data1D::nXAxisPoints() const
{
	return x_.nItems();
}

// Return x axis value specified
double Data1D::xAxis(int index) const
{
	return x_.constAt(index);
}

// Return minimum (first) x axis point
double Data1D::xAxisMin() const
{
	return x_.firstValue();
}

// Return maximum (last) x axis point
double Data1D::xAxisMax() const
{
	return x_.lastValue();
}

// Return x axis Array
const Array<double>& Data1D::xAxis() const
{
	return x_;
}

// Return number of datapoints present in whole dataset
int Data1D::nDataPoints() const
{
	return x_.nItems();
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* Data1D::itemClassName()
{
	return "Data1D";
}

// Write data through specified LineParser
bool Data1D::write(LineParser& parser)
{
	if (!parser.writeLineF("%s\n", objectTag())) return false;
	if (!parser.writeLineF("%i\n", x_.nItems())) return false;
	for (int n=0; n<x_.nItems(); ++n)
	{
		if (!parser.writeLineF("%f\n", x_[n])) return false;
		if (!y_.at(n).write(parser)) return false;
	}
	return true;
}

// Read data through specified LineParser
bool Data1D::read(LineParser& parser)
{
	clear();

	if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) return false;
	setObjectTag(parser.line());

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nPoints = parser.argi(0);
	x_.createEmpty(nPoints);
	y_.createEmpty(nPoints);

	for (int n=0; n<nPoints; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		x_[n] = parser.argd(0);
		if (!y_[n].read(parser)) return false;
	}

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool Data1D::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	if (!procPool.broadcast(x_, rootRank)) return false;
	for (int n=0; n<y_.nItems(); ++n) if (!y_[n].broadcast(procPool, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool Data1D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(x_)) return Messenger::error("Data1D x axis values not equivalent.\n");
	for (int n=0; n<y_.nItems(); ++n) if (!y_[n].equality(procPool)) return Messenger::error("Data1D values not equivalent.\n");
#endif
	return true;
}
