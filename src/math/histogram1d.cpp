/*
	*** 1-Dimensional Binned Data With Statistics
	*** src/math/histogram1d.cpp
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

#include "math/histogram1d.h"
#include "base/messenger.h"
#include "base/lineparser.h"

// Static Members (ObjectStore)
template<class Histogram1D> RefList<Histogram1D,int> ObjectStore<Histogram1D>::objects_;
template<class Histogram1D> int ObjectStore<Histogram1D>::objectCount_ = 0;
template<class Histogram1D> int ObjectStore<Histogram1D>::objectType_ = ObjectInfo::Histogram1DObject;
template<class Histogram1D> const char* ObjectStore<Histogram1D>::objectTypeName_ = "Histogram1D";

// Constructor
Histogram1D::Histogram1D() : ListItem<Histogram1D>(), ObjectStore<Histogram1D>(this) 
{
	clear();
}

// Destructor
Histogram1D::~Histogram1D()
{
}

// Copy Constructor
Histogram1D::Histogram1D(const Histogram1D& source) : ObjectStore<Histogram1D>(this)
{
	(*this) = source;
}

// Clear Data
void Histogram1D::clear()
{
	xMin_ = 0.0;
	xMax_ = 0.0;
	binWidth_ = 0.0;
	nBins_ = 0;
	nBinned_ = 0;
	x_.clear();
	y_.clear();
	yAccumulated_.clear();
}

/*
 * Data
 */

// Initialise with specified bin range
void Histogram1D::initialise(double xMin, double xMax, double binWidth)
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
		++nBins_;
		xMax_ = xMin_ + nBins_*binWidth_;
	}
	else xMax_ = xMax;

	// Create the arrays
	x_.initialise(nBins_);
	y_.initialise(nBins_);
	yAccumulated_.initialise(nBins_);

	// Create centre-bin array
	double xCentre = xMin + binWidth_*0.5;
	for (int n=0; n<nBins_; ++n, xCentre += binWidth_) x_[n] = xCentre;
}

// Reset histogram bins
void Histogram1D::resetBins()
{
	y_ = 0;
	nBinned_ = 0;
}

// Reset all accumulated data
void Histogram1D::resetAll()
{
	y_ = 0;
	yAccumulated_ = 0.0;
	nBinned_ = 0;
}

// Return minimum x value
double Histogram1D::xMin() const
{
	return xMin_;
}

// Return maximum x value
double Histogram1D::xMax() const
{
	return xMax_;
}

// Return bin width
double Histogram1D::binWidth() const
{
	return binWidth_;
}

// Return number of bins
int Histogram1D::nBins() const
{
	return nBins_;
}

// Increase bin value at specified x
void Histogram1D::bin(double x, int n)
{
	// Calculate target bin
	int bin = (x - xMin_) / binWidth_;

	// Check bin range
	if ((bin < 0) || (bin >= nBins_)) return;

	y_[bin] += n;
}

// Return Array of x centre-bin values
const Array<double>& Histogram1D::x() const
{
	return x_;
}

/*
 * Statistics
 */

// Add current histogram populations in to statistics
void Histogram1D::accumulate()
{
	for (int n=0; n<nBins_; ++n) yAccumulated_[n] += y_[n];

	for (int n=0; n<nBins_; ++n) printf(" %f  %f\n", x_[n], yAccumulated_[n].value());
}

// Return statistics array
Array<SampledDouble>& Histogram1D::yAccumulated()
{
	return yAccumulated_;
}

// Return statistics array (const)
const Array<SampledDouble>& Histogram1D::yAccumulated() const
{
	return yAccumulated_;
}

/*
 * Operators
 */

// Operator =
void Histogram1D::operator=(const Histogram1D& source)
{
	xMin_ = source.xMin_;
	xMax_ = source.xMax_;
	nBins_ = source.nBins_;
	binWidth_ = source.binWidth_;
	nBinned_ = source.nBinned_;
	x_ = source.x_;
	yAccumulated_ = source.yAccumulated_;
	y_ = source.y_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* Histogram1D::itemClassName()
{
	return "Histogram1D";
}

// Write data through specified LineParser
bool Histogram1D::write(LineParser& parser)
{
	if (!parser.writeLineF("%s\n", objectName())) return false;
	if (!parser.writeLineF("%f %f %f\n", xMin_, xMax_, binWidth_)) return false;
	if (!parser.writeLineF("%i\n", nBinned_)) return false;
	for (int n=0; n<nBins_; ++n) if (!yAccumulated_.at(n).write(parser)) return false;
	return true;
}

// Read data through specified LineParser
bool Histogram1D::read(LineParser& parser)
{
	clear();

	if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) return false;
	setObjectName(parser.line());

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	initialise(parser.argd(0), parser.argd(1), parser.argd(2));

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	nBinned_ = parser.argi(0);

	for (int n=0; n<nBins_; ++n) if (!yAccumulated_.at(n).read(parser)) return false;

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast data
bool Histogram1D::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// Range data
	if (!procPool.broadcast(xMin_, rootRank)) return false;
	if (!procPool.broadcast(xMax_, rootRank)) return false;
	if (!procPool.broadcast(binWidth_, rootRank)) return false;
	if (!procPool.broadcast(nBins_, rootRank)) return false;

	// Data
	if (!procPool.broadcast(nBinned_, rootRank)) return false;
	if (!procPool.broadcast(x_, rootRank)) return false;
	if (!procPool.broadcast(y_, rootRank)) return false;
	if (procPool.poolRank() != rootRank) yAccumulated_.initialise(nBins_);
	for (int n=0; n<nBins_; ++n) if (!yAccumulated_[n].broadcast(procPool, rootRank)) return false;
#endif
	return true;
}

// Check item equality
bool Histogram1D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Check number of items in arrays first
	if (!procPool.equality(xMin_)) return Messenger::error("Histogram1D xMin is not equivalent (process %i has %e).\n", procPool.poolRank(), xMin_);
	if (!procPool.equality(xMax_)) return Messenger::error("Histogram1D xMax is not equivalent (process %i has %e).\n", procPool.poolRank(), xMax_);
	if (!procPool.equality(binWidth_)) return Messenger::error("Histogram1D bin width is not equivalent (process %i has %e).\n", procPool.poolRank(), binWidth_);
	if (!procPool.equality(nBins_)) return Messenger::error("Histogram1D number of bins is not equivalent (process %i has %i).\n", procPool.poolRank(), nBins_);
	if (!procPool.equality(x_)) return Messenger::error("Histogram1D x values not equivalent.\n");
	if (!procPool.equality(y_)) return Messenger::error("Histogram1D y values not equivalent.\n");
	if (!procPool.equality(nBinned_)) return Messenger::error("Histogram1D nunmber of binned values is not equivalent (process %i has %i).\n", procPool.poolRank(), nBinned_);
	for (int n=0; n<nBins_; ++n) if (!yAccumulated_[n].equality(procPool)) return Messenger::error("Histogram1D accumulated y values not equivalent.\n");
#endif
	return true;
}
