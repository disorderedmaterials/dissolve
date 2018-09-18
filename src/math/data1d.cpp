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
Data1D::Data1D() : ListItem<Data1D>(), ObjectStore<Data1D>(this), Plottable(Plottable::OneAxisPlottable)
{
	hasError_ = false;

	clear();
}

// Destructor
Data1D::~Data1D()
{
}

// Copy Constructor
Data1D::Data1D(const Data1D& source) : ObjectStore<Data1D>(this), Plottable(Plottable::OneAxisPlottable)
{
	(*this) = source;
}

// Clear Data
void Data1D::clear()
{
	x_.clear();
	values_.clear();
	errors_.clear();
}

/*
 * Data
 */

// Initialise arrays to specified size
void Data1D::initialise(int size, bool withError)
{
	x_.initialise(size);
	values_.initialise(size);
	hasError_ = withError;
	if (hasError_) errors_.initialise(size);
	else errors_.clear();
}

// Initialise to be consistent in size and x axis with supplied object
void Data1D::initialise(const Data1D& source)
{
	x_ = source.x_;
	values_.initialise(x_.nItems());
	hasError_ = source.hasError_;
	if (hasError_) errors_.initialise(x_.nItems());
	else errors_.clear();
}

// Copy arrays from supplied object
void Data1D::copyArrays(const Data1D& source)
{
	x_ = source.x_;
	values_ = source.values_;
	errors_ = source.errors_;
	hasError_ = source.hasError_;
}

// Zero values array
void Data1D::zero()
{
	values_ = 0.0;
	if (hasError_) errors_ = 0.0;
}

// Add new data point
void Data1D::addPoint(double x, double value)
{
	x_.add(x);
	values_.add(value);

	if (hasError_) errors_.add(0.0);
}

// Add new data point with error
void Data1D::addPoint(double x, double value, double error)
{
	x_.add(x);
	values_.add(value);

	if (hasError_) errors_.add(error);
	else Messenger::warn("Tried to addPoint() with an error to Data1D, but this Data1D () has no error information associated with it.\n", name(), objectTag());
}

// Return x value specified
double& Data1D::xAxis(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		static double dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in Data1D::xAxis().\n", index);
		return dummy;
	}
#endif
	return x_[index];
}

// Return x value specified (const)
double Data1D::constXAxis(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= x_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for x_ array in Data1D::constXAxis().\n", index);
		return 0.0;
	}
#endif
	return x_.constAt(index);
}

// Return x Array
Array<double>& Data1D::xAxis()
{
	return x_;
}

// Return x axis Array (const)
const Array<double>& Data1D::constXAxis() const
{
	return x_;
}

// Return y value specified
double& Data1D::value(int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= values_.nItems()))
	{
		static double dummy;
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for values_ array in Data1D::value().\n", index);
		return dummy;
	}
#endif
	return values_[index];
}

// Return y value specified (const)
double Data1D::constValue(int index) const
{
#ifdef CHECKS
	if ((index < 0) || (index >= values_.nItems()))
	{
		Messenger::error("OUT_OF_RANGE - Index %i is out of range for values_ array in Data1D::constValue().\n", index);
		return 0.0;
	}
#endif
	return values_.constAt(index);
}

// Return y Array
Array<double>& Data1D::values()
{
	return values_;
}

// Return y Array (const)
const Array<double>& Data1D::constValues() const
{
	return values_;
}


// Return number of values present in whole dataset
int Data1D::nValues() const
{
	return x_.nItems();
}

// Return minimum value over all data points
double Data1D::minValue() const
{
	if (values_.nItems() == 0) return 0.0;

	double value = values_.constAt(0);
	for (int n=1; n<values_.nItems(); ++n) if (values_.constAt(n) < value) value = values_.constAt(n);

	return value;
}

// Return maximum value over all data points
double Data1D::maxValue() const
{
	if (values_.nItems() == 0) return 0.0;

	double value = values_.constAt(0);
	for (int n=1; n<values_.nItems(); ++n) if (values_.constAt(n) > value) value = values_.constAt(n);

	return value;
}

// Add / initialise errors array
void Data1D::addErrors()
{
	if (hasError_) Messenger::warn("Adding an error array to a Data1D that already has one...\n");

	errors_.initialise(x_.nItems());

	hasError_ = true;
}

// Return whether the values have associated errors
bool Data1D::valuesHaveErrors() const
{
	return hasError_;
}

// Return error value specified
double& Data1D::error(int index)
{
	if (!hasError_)
	{
		static double dummy;
		Messenger::warn("This Data1D (name='%s', tag='%s') has no errors to return, but error(int) was requested.\n", name(), objectTag());
		return dummy;
	}
	
	return errors_.at(index);
}

// Return error value specified (const)
double Data1D::constError(int index) const
{
	if (!hasError_)
	{
		Messenger::warn("This Data1D (name='%s', tag='%s') has no errors to return, but constError(int) was requested.\n", name(), objectTag());
		return 0.0;
	}
	
	return errors_.constAt(index);
}

// Return error Array
Array<double>& Data1D::errors()
{
	if (!hasError_) Messenger::warn("This Data1D (name='%s', tag='%s') has no errors to return, but errors() was requested.\n", name(), objectTag());
	
	return errors_;
}

// Return error Array (const)
const Array<double>& Data1D::constErrors() const
{
	if (!hasError_) Messenger::warn("This Data1D (name='%s', tag='%s') has no errors to return, but constErrors() was requested.\n", name(), objectTag());
	
	return errors_;
}

/*
 * Operators
 */

// Operator =
void Data1D::operator=(const Data1D& source)
{
	name_ = source.name_;
	x_ = source.x_;
	values_ = source.values_;
	hasError_ = source.hasError_;
	errors_ = source.errors_;
}

// Operator +=
void Data1D::operator+=(const Data1D& source)
{
	// If no data is present, simply copy the other arrays
	if (x_.nItems() == 0)
	{
		copyArrays(source);
		return;
	}

	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't += these Data1D together since they are of differing sizes.\n");
		return;
	}

	// Loop over points, summing them into our array
	for (int n=0; n<x_.nItems(); ++n)
	{
#ifdef CHECKS
		// Check x values for consistency
		if (fabs(x_[n] - source.constXAxis(n)) > 1.0e-6)
		{
			Messenger::error("Failed to += these Data1D together since the x arrays are different (at point %i, x are %e and %e).\n", n, x_[n], source.constXAxis(n));
			return;
		}
#endif
		values_[n] += source.constValue(n);
	}
}

// Operator +=
void Data1D::operator+=(const double delta)
{
	for (int n=0; n<values_.nItems(); ++n) values_[n] += delta;
}

// Operator -=
void Data1D::operator-=(const Data1D& source)
{
	// If no data is present, simply copy the other arrays and negate the y array
	if (x_.nItems() == 0)
	{
		copyArrays(source);
		for (int n=0; n<values_.nItems(); ++n) values_[n] = -values_[n];
		return;
	}

	// Check array sizes
	if (x_.nItems() != source.x_.nItems())
	{
		Messenger::error("Can't -= these Data1D together since they are of differing sizes.\n");
		return;
	}

	// Loop over points, summing them into our array
	for (int n=0; n<x_.nItems(); ++n)
	{
#ifdef CHECKS
		// Check x values for consistency
		if (fabs(x_[n] - source.constXAxis(n)) > 1.0e-6)
		{
			Messenger::error("Failed to -= these Data1D together since the x arrays are different (at point %i, x are %e and %e).\n", n, x_[n], source.constXAxis(n));
			return;
		}
#endif
		values_[n] -= source.constValue(n);
	}
}

// Operator -=
void Data1D::operator-=(const double delta)
{
	for (int n=0; n<values_.nItems(); ++n) values_[n] -= delta;
}

// Operator *=
void Data1D::operator*=(const double factor)
{
	values_ *= factor;
	if (hasError_) errors_ *= factor;
}

// Operator /=
void Data1D::operator/=(const double factor)
{
	values_ /= factor;
	if (hasError_) errors_ /= factor;
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

	Messenger::printVerbose("Read %i points from '%s' (columns %i and %i).\n", nValues(), parser.inputFilename(), xcol+1, ycol+1);

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
	
	if (hasError_)
	{
		for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e  %16.10e\n", x_.constAt(n), values_.constAt(n), errors_.constAt(n));
	}
	else for (int n = 0; n<x_.nItems(); ++n) parser.writeLineF("%16.10e  %16.10e\n", x_.constAt(n), values_.constAt(n));

	parser.closeFiles();

	return true;
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
	if (!parser.writeLineF("%i %i\n", x_.nItems(), hasError_)) return false;
	if (hasError_)
	{
		for (int n=0; n<x_.nItems(); ++n) if (!parser.writeLineF("%f  %f  %f\n", x_[n], values_[n], errors_[n])) return false;
	}
	else for (int n=0; n<x_.nItems(); ++n) if (!parser.writeLineF("%f  %f\n", x_[n], values_[n])) return false;

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
	bool errors = parser.argb(1);
	initialise(nPoints, errors);

	for (int n=0; n<nPoints; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		x_[n] = parser.argd(0);
		values_[n] = parser.argd(1);
		if (hasError_) errors_[n] = parser.argd(2);
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
	if (!procPool.broadcast(values_, rootRank)) return false;
	if (!procPool.broadcast(hasError_, rootRank)) return false;
	if (!procPool.broadcast(errors_, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool Data1D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(x_)) return Messenger::error("Data1D x axis values not equivalent.\n");
	if (!procPool.equality(values_)) return Messenger::error("Data1D y axis values not equivalent.\n");
	if (!procPool.equality(hasError_)) return Messenger::error("Data1D error flag not equivalent.\n");
	if (!procPool.equality(errors_)) return Messenger::error("Data1D error values not equivalent.\n");
#endif
	return true;
}
