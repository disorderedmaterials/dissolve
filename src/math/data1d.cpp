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
	xAxis_.clear();
	values_.clear();
}

/*
 * Data
 */

// Initialise to be consistent with supplied Histogram1D object
void Data1D::initialise(const Histogram1D& source)
{
	clear();

	xAxis_ = source.binCentres();
	values_.initialise(xAxis_.nItems());
}

// Zero values array
void Data1D::zero()
{
	values_ = 0.0;
}

// Accumulate specified histogram data
void Data1D::accumulate(const Histogram1D& source)
{
}

// Return number of values along x
int Data1D::nX() const
{
	return xAxis_.nItems();
}

// Return x axis Array
const Array<double>& Data1D::xAxis() const
{
	return xAxis_;
}

// Return Array of accumulated x values
const Array<SampledDouble>& Data1D::values() const
{
	return values_;
}

/*
 * Operators
 */

// Operator =
void Data1D::operator=(const Data1D& source)
{
	xAxis_ = source.xAxis_;
	values_ = source.values_;
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
	if (!parser.writeLineF("%s\n", objectName())) return false;
	if (!parser.writeLineF("%i\n", xAxis_.nItems())) return false;
	for (int n=0; n<xAxis_.nItems(); ++n)
	{
		if (!parser.writeLineF("%f\n", xAxis_[n])) return false;
		if (!values_.at(n).write(parser)) return false;
	}
	return true;
}

// Read data through specified LineParser
bool Data1D::read(LineParser& parser)
{
	clear();

	if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) return false;
	setObjectName(parser.line());

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nPoints = parser.argi(0);
	xAxis_.createEmpty(nPoints);
	values_.createEmpty(nPoints);

	for (int n=0; n<nPoints; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		xAxis_[n] = parser.argd(0);
		if (!values_[n].read(parser)) return false;
	}

	return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Data1D::allSum(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.allSum(bins_, nBins_)) return false;
#endif

	return true;
}

// Broadcast data
bool Data1D::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	if (!procPool.broadcast(xAxis_, rootRank)) return false;
	for (int n=0; n<values_.nItems(); ++n) if (!values_[n].broadcast(procPool, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool Data1D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.equality(xAxis_)) return Messenger::error("Data1D x axis values not equivalent.\n");
	for (int n=0; n<values_.nItems(); ++n) if (!values_[n].equality(procPool)) return Messenger::error("Data1D values not equivalent.\n");
#endif
	return true;
}
