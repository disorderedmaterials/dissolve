/*
	*** 1-Dimensional Binned Data With Statistics
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

#include "base/processpool.h"
#include "math/xydata.h"
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
	xMin_ = 0.0;
	xMax_ = 0.0;
	binWidth_ = 0.0;
	nBins_ = 0;
	nSamples_ = 0;
	normalisationFactor_ = 1.0;
	y_.clear();
}

/*
 * Data
 */

// Initialise with specified bin range
void Data1D::initialise(double xMin, double xMax, double binWidth)
{
	clear();

	// Store xMin and binWidth
	xMin_ = xMin;
	binWidth_ = binWidth;

	// Clamp xMax_ to nearest bin boundary (not less than the supplied xMax)
	double range = xMax - xMin_;
	nBins_ = range / binWidth_;
	if ((xMin_ + nBins_*binWidth_) < xMax)
	{
		++nBins;
		xMax_ = xMin_ + nBins_*binWidth_;
	}
	else xMax_ = xMax;

	// Create the arrays
	x_.initialise(nBins_);
	y_.initialise(nBins_);

	// Create centre-bin array
	double xCentre = xMin + binWidth_*0.5;
	for (int n=0; n<nBins_; ++n, xCentre += binWidth_) x_.add(xCentre);
}

// Reset all accumulated data
void Data1D::reset()
{
	y_ = 0.0;
	nSamples_ = 0;
}

// Return minimum x value
double Data1D::xMin() const
{
	return xMin_;
}

// Return maximum x value
double Data1D::xMax() const
{
	return xMax_;
}

// Return bin width
double Data1D::binWidth() const
{
	return binWidth_;
}

// Return number of bins
int Data1D::nBins() const
{
	return nBins_;
}

// Return Array of x centre-bin values
const Array<double>& Data1D::x() const
{
	interpolationInterval_ = -1;
	return x_;
}

/*
 * Operators
 */

// Operator =
void Data1D::operator=(const Data1D& source)
{
	xMin_ = source.xMin_;
	xMax_ = source.xMax_;
	nBins_ = source.nBins_;
	binWidth_ = source.binWidth_;
	nSamples_ = source.nSamples_;
	normalisationFactor_ = source.normalisationFactor_;
	x_ = source.x_;
	y_ = source.y_;
}

// Operator *=
void Data1D::operator*=(const double factor)
{
	for (int n=0; n<y_.nItems(); ++n) y_[n] *= factor;
}

// Operator /=
void Data1D::operator/=(const double factor)
{
	for (int n=0; n<y_.nItems(); ++n) y_[n] /= factor;
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
	if (!parser.writeLineF("%f %f %f\n", xMin_, xMax_, binWidth_)) return false;
	if (!parser.writeLineF("%i %i\n", nBins_, nSamples_)) return false;
	for (int n=0; n<nBins_; ++n) if (!y_.value(n).write(parser)) return false;
	return true;
}

// Read data through specified LineParser
bool Data1D::read(LineParser& parser)
{
	clear();
	XXX
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
bool Data1D::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// Range data
	if (!procPool.broadcast(xMin_, rootRank)) return false;
	if (!procPool.broadcast(xMax_, rootRank)) return false;
	if (!procPool.broadcast(binWidth_, rootRank)) return false;
	if (!procPool.broadcast(nBins_, rootRank)) return false;

	// Data
	if (!procPool.broadcast(nSamples_, rootRank)) return false;
	if (!procPool.broadcast(x_, rootRank)) return false;
	if (!procPool.broadcast(y_, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool Data1D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Check number of items in arrays first
	if (!procPool.equality(nPoints())) return Messenger::error("Data1D number of points is not equivalent (process %i has %i).\n", procPool.poolRank(), nPoints());
	for (int n=0; n<nPoints(); ++n)
	{
		if (!procPool.equality(x_[n])) return Messenger::error("Data1D x value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), x_[n]);
		if (!procPool.equality(y_[n])) return Messenger::error("Data1D y value %i is not equivalent (process %i has %e).\n", n, procPool.poolRank(), y_[n]);
	}
	if (!procPool.equality(z_)) return Messenger::error("Data1D z value is not equivalent (process %i has %e).\n", procPool.poolRank(), z_);
#endif
	return true;
}
