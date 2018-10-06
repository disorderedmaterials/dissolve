/*
	*** 3-Dimensional Histogram
	*** src/math/histogram3d.cpp
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

#include "math/histogram3d.h"
#include "math/histogram1d.h"
#include "base/messenger.h"
#include "base/lineparser.h"

// Static Members (ObjectStore)
template<class Histogram3D> RefList<Histogram3D,int> ObjectStore<Histogram3D>::objects_;
template<class Histogram3D> int ObjectStore<Histogram3D>::objectCount_ = 0;
template<class Histogram3D> int ObjectStore<Histogram3D>::objectType_ = ObjectInfo::Histogram3DObject;
template<class Histogram3D> const char* ObjectStore<Histogram3D>::objectTypeName_ = "Histogram3D";

// Constructor
Histogram3D::Histogram3D() : ListItem<Histogram3D>(), ObjectStore<Histogram3D>(this) 
{
	accumulatedData_.addErrors();

	clear();
}

// Destructor
Histogram3D::~Histogram3D()
{
}

// Copy Constructor
Histogram3D::Histogram3D(const Histogram3D& source) : ObjectStore<Histogram3D>(this)
{
	(*this) = source;
}

// Clear Data
void Histogram3D::clear()
{
	xMinimum_ = 0.0;
	xMaximum_ = 0.0;
	xBinWidth_ = 0.0;
	nXBins_ = 0;
	yMinimum_ = 0.0;
	yMaximum_ = 0.0;
	yBinWidth_ = 0.0;
	nYBins_ = 0;
	zMinimum_ = 0.0;
	zMaximum_ = 0.0;
	zBinWidth_ = 0.0;
	nZBins_ = 0;
	nBinned_ = 0;
	nMissed_ = 0;
	bins_.clear();
	bins_.clear();
	xBinCentres_.clear();
	yBinCentres_.clear();
	zBinCentres_.clear();
	averages_.clear();
}

/*
 * Data
 */

// Update accumulated data
void Histogram3D::updateAccumulatedData()
{
	// Set up arrays
	accumulatedData_.initialise(nXBins_, nYBins_, nZBins_);

	// Store bin centres and accumulated averages in the object
	for (int x=0; x<nXBins_; ++x)
	{
		accumulatedData_.xAxis(x) = xBinCentres_[x];
		for (int y=0; y<nYBins_; ++y)
		{
			if (x == 0) accumulatedData_.yAxis(y) = yBinCentres_[y];

			for (int z=0; z<nZBins_; ++z)
			{
				if (x == 0) accumulatedData_.zAxis(z) = zBinCentres_[z];

				accumulatedData_.value(x, y, z) = averages_.constAt(x, y, z);
				accumulatedData_.error(x, y, z) = averages_.constAt(x, y, z).stDev();
			}
		}
	}
}

// Initialise with specified bin range
void Histogram3D::initialise(double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth, double zMin, double zMax, double zBinWidth)
{
	clear();

	// Set up x axis
	xMinimum_ = xMin;
	xMaximum_ = xMax;
	xBinWidth_ = xBinWidth;
	Histogram1D::setUpAxis(xMinimum_, xMaximum_, xBinWidth_, nXBins_, xBinCentres_);

	// Set up y axis
	yMinimum_ = yMin;
	yMaximum_ = yMax;
	yBinWidth_ = yBinWidth;
	Histogram1D::setUpAxis(yMinimum_, yMaximum_, yBinWidth_, nYBins_, yBinCentres_);

	// Set up z axis
	zMinimum_ = zMin;
	zMaximum_ = zMax;
	zBinWidth_ = zBinWidth;
	Histogram1D::setUpAxis(zMinimum_, zMaximum_, zBinWidth_, nZBins_, zBinCentres_);

	// Create the main bins array
	bins_.initialise(nXBins_, nYBins_, nZBins_);
	averages_.initialise(nXBins_, nYBins_, nZBins_);
}

// Zero histogram bins
void Histogram3D::zeroBins()
{
	bins_ = 0;
	nBinned_ = 0;
	nMissed_ = 0;
}

// Return minimum value for x data (hard left-edge of first bin)
double Histogram3D::xMinimum() const
{
	return xMinimum_;
}

// Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::xMaximum() const
{
	return xMaximum_;
}

// Return x bin width
double Histogram3D::xBinWidth() const
{
	return xBinWidth_;
}

// Return number of x bins
int Histogram3D::nXBins() const
{
	return nXBins_;
}

// Return minimum value for y data (hard left-edge of first bin)
double Histogram3D::yMinimum() const
{
	return yMinimum_;
}

// Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::yMaximum() const
{
	return yMaximum_;
}

// Return y bin width
double Histogram3D::yBinWidth() const
{
	return yBinWidth_;
}

// Return number of y bins
int Histogram3D::nYBins() const
{
	return nYBins_;
}

// Bin specified value
void Histogram3D::bin(double x, double y, double z)
{
	// Calculate target bin along x
	int xBin = (x - xMinimum_) / xBinWidth_;
	if ((xBin < 0) || (xBin >= nXBins_))
	{
		++nMissed_;
		return;
	}

	// Calculate target bin along y
	int yBin = (y - yMinimum_) / yBinWidth_;
	if ((yBin < 0) || (yBin >= nYBins_))
	{
		++nMissed_;
		return;
	}

	// Calculate target bin along z
	int zBin = (z - zMinimum_) / zBinWidth_;
	if ((zBin < 0) || (zBin >= nZBins_))
	{
		++nMissed_;
		return;
	}

	++bins_.at(xBin, yBin, zBin);
	++nBinned_;
}

// Bin specified value (as Vec3)
void Histogram3D::bin(Vec3<double> v)
{
	bin(v.x, v.y, v.z);
}

// Accumulate current histogram bins into averages
void Histogram3D::accumulate()
{
	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y)
		{
			for (int z=0; z<nZBins_; ++z) averages_.at(x,y,z) += double(bins_.at(x,y,z));
		}
	}

	// Update accumulated data
	updateAccumulatedData();
}

// Return Array of x centre-bin values
const Array<double>& Histogram3D::xBinCentres() const
{
	return xBinCentres_;
}

// Return Array of y centre-bin values
const Array<double>& Histogram3D::yBinCentres() const
{
	return yBinCentres_;
}

// Return histogram data
Array3D<long int>& Histogram3D::bins()
{
	return bins_;
}

// Add source histogram data into local array
void Histogram3D::add(Histogram3D& other, int factor)
{
	if ((nXBins_ != other.nXBins_) || (nYBins_ != other.nYBins_))
	{
		Messenger::print("BAD_USAGE - Can't add Histogram3D data since arrays are not the same size (%ix%i vs %ix%i).\n", nXBins_, nYBins_, other.nXBins_, other.nYBins_);
		return;
	}

	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y)
		{
			for (int z=0; z<nZBins_; ++z) bins_.at(x,y,z) += other.bins_.at(x,y,z) * factor;
		}
	}
}

// Return accumulated (averaged) data
const Data3D& Histogram3D::accumulatedData() const
{
	return accumulatedData_;
}

/*
 * Operators
 */

// Operator =
void Histogram3D::operator=(const Histogram3D& source)
{
	xMinimum_ = source.xMinimum_;
	xMaximum_ = source.xMaximum_;
	xBinWidth_ = source.xBinWidth_;
	nXBins_ = source.nXBins_;
	yMinimum_ = source.yMinimum_;
	yMaximum_ = source.yMaximum_;
	yBinWidth_ = source.yBinWidth_;
	nYBins_ = source.nYBins_;
	nBinned_ = source.nBinned_;
	nMissed_ = source.nMissed_;
	bins_ = source.bins_;
	xBinCentres_ = source.xBinCentres_;
	yBinCentres_ = source.yBinCentres_;
	averages_ = source.averages_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
const char* Histogram3D::itemClassName()
{
	return "Histogram3D";
}

// Write data through specified LineParser
bool Histogram3D::write(LineParser& parser)
{
	if (!parser.writeLineF("%s\n", objectTag())) return false;
	if (!parser.writeLineF("%f %f %f %f %f %f %f %f %f\n", xMinimum_, xMaximum_, xBinWidth_, yMinimum_, yMaximum_, yBinWidth_, zMinimum_, zMaximum_, zBinWidth_)) return false;
	if (!parser.writeLineF("%li  %li\n", nBinned_, nMissed_)) return false;
	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y)
		{
			for (int z=0; z<nZBins_; ++z) if (!averages_.at(x,y,z).write(parser)) return false;
		}
	}

	return true;
}

// Read data through specified LineParser
bool Histogram3D::read(LineParser& parser)
{
	clear();

	if (parser.readNextLine(LineParser::Defaults) != LineParser::Success) return false;
	setObjectTag(parser.line());

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	initialise(parser.argd(0), parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6), parser.argd(7), parser.argd(8));

	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	nBinned_ = parser.argli(0);
	nMissed_ = parser.argli(1);

	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y)
		{
			for (int z=0; z<nZBins_; ++z) if (!averages_.at(x,y,z).read(parser)) return false;
		}
	}

	return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram3D::allSum(ProcessPool& procPool)
{
#ifdef PARALLEL
	if (!procPool.allSum(bins_, nBins_)) return false;
#endif

	return true;
}

// Broadcast data
bool Histogram3D::broadcast(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	// Range data
	if (!procPool.broadcast(xMinimum_, rootRank)) return false;
	if (!procPool.broadcast(xMaximum_, rootRank)) return false;
	if (!procPool.broadcast(xBinWidth_, rootRank)) return false;
	if (!procPool.broadcast(nXBins_, rootRank)) return false;
	if (!procPool.broadcast(yMinimum_, rootRank)) return false;
	if (!procPool.broadcast(yMaximum_, rootRank)) return false;
	if (!procPool.broadcast(yBinWidth_, rootRank)) return false;
	if (!procPool.broadcast(nYBins_, rootRank)) return false;

	// Data
	if (!procPool.broadcast(nBinned_, rootRank)) return false;
	if (!procPool.broadcast(nMissed_, rootRank)) return false;
	if (!procPool.broadcast(xBinCentres_, rootRank)) return false;
	if (!procPool.broadcast(yBinCentres_, rootRank)) return false;
	if (!procPool.broadcast(bins_, rootRank)) return false;
	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y) if (!averages_.at(x,y).broadcast(procPool, rootRank)) return false;
	}
#endif
	return true;
}

// Check item equality
bool Histogram3D::equality(ProcessPool& procPool)
{
#ifdef PARALLEL
	// Check number of items in arrays first
	if (!procPool.equality(xMinimum_)) return Messenger::error("Histogram3D minimum x value is not equivalent (process %i has %e).\n", procPool.poolRank(), minimum_);
	if (!procPool.equality(xMaximum_)) return Messenger::error("Histogram3D maximum x value is not equivalent (process %i has %e).\n", procPool.poolRank(), maximum_);
	if (!procPool.equality(xBinWidth_)) return Messenger::error("Histogram3D bin x width is not equivalent (process %i has %e).\n", procPool.poolRank(), binWidth_);
	if (!procPool.equality(nXBins_)) return Messenger::error("Histogram3D number of x bins is not equivalent (process %i has %i).\n", procPool.poolRank(), nBins_);
	if (!procPool.equality(xBinCentres_)) return Messenger::error("Histogram3D x bin centre values not equivalent.\n");
	if (!procPool.equality(yMinimum_)) return Messenger::error("Histogram3D minimum y value is not equivalent (process %i has %e).\n", procPool.poolRank(), minimum_);
	if (!procPool.equality(yMaximum_)) return Messenger::error("Histogram3D maximum y value is not equivalent (process %i has %e).\n", procPool.poolRank(), maximum_);
	if (!procPool.equality(yBinWidth_)) return Messenger::error("Histogram3D bin y width is not equivalent (process %i has %e).\n", procPool.poolRank(), binWidth_);
	if (!procPool.equality(nYBins_)) return Messenger::error("Histogram3D number of y bins is not equivalent (process %i has %i).\n", procPool.poolRank(), nBins_);
	if (!procPool.equality(yBinCentres_)) return Messenger::error("Histogram3D y bin centre values not equivalent.\n");
	if (!procPool.equality(bins_)) return Messenger::error("Histogram3D bin values not equivalent.\n");
	if (!procPool.equality(nBinned_)) return Messenger::error("Histogram3D nunmber of binned values is not equivalent (process %i has %li).\n", procPool.poolRank(), nBinned_);
	if (!procPool.equality(nMissed_)) return Messenger::error("Histogram3D nunmber of binned values is not equivalent (process %i has %li).\n", procPool.poolRank(), nBinned_);
	for (int x=0; x<nXBins_; ++x)
	{
		for (int y=0; y<nYBins_; ++y) if (!averages_.at(x,y).equality(procPool)) return Messenger::error("Histogram3D average values not equivalent.\n");
	}
#endif
	return true;
}
