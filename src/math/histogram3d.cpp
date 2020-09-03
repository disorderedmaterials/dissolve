/*
    *** 3-Dimensional Histogram
    *** src/math/histogram3d.cpp
    Copyright T. Youngs 2012-2020

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
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/histogram1d.h"

// Static Members (ObjectStore)
template <class Histogram3D> RefDataList<Histogram3D, int> ObjectStore<Histogram3D>::objects_;
template <class Histogram3D> int ObjectStore<Histogram3D>::objectCount_ = 0;
template <class Histogram3D> int ObjectStore<Histogram3D>::objectType_ = ObjectInfo::Histogram3DObject;
template <class Histogram3D> std::string_view ObjectStore<Histogram3D>::objectTypeName_ = "Histogram3D";

Histogram3D::Histogram3D() : ListItem<Histogram3D>(), ObjectStore<Histogram3D>(this)
{
    accumulatedData_.addErrors();

    clear();
}

Histogram3D::~Histogram3D() {}

Histogram3D::Histogram3D(const Histogram3D &source) : ObjectStore<Histogram3D>(this) { (*this) = source; }

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
    // Store bin centres and accumulated averages in the object
    for (int x = 0; x < nXBins_; ++x)
    {
        accumulatedData_.xAxis(x) = xBinCentres_[x];
        for (int y = 0; y < nYBins_; ++y)
        {
            if (x == 0)
                accumulatedData_.yAxis(y) = yBinCentres_[y];

            for (int z = 0; z < nZBins_; ++z)
            {
                if (x == 0)
                    accumulatedData_.zAxis(z) = zBinCentres_[z];

                accumulatedData_.value(x, y, z) = averages_.constAt(x, y, z);
                accumulatedData_.error(x, y, z) = averages_.constAt(x, y, z).stDev();
            }
        }
    }
}

// Initialise with specified bin range
void Histogram3D::initialise(double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth,
                             double zMin, double zMax, double zBinWidth)
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

    // Set up the accumulated data array
    accumulatedData_.initialise(nXBins_, nYBins_, nZBins_, true);
    accumulatedData_.xAxis() = xBinCentres_;
    accumulatedData_.yAxis() = yBinCentres_;
    accumulatedData_.zAxis() = zBinCentres_;
}

// Zero histogram bins
void Histogram3D::zeroBins()
{
    bins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
}

// Return minimum value for x data (hard left-edge of first bin)
double Histogram3D::xMinimum() const { return xMinimum_; }

// Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::xMaximum() const { return xMaximum_; }

// Return x bin width
double Histogram3D::xBinWidth() const { return xBinWidth_; }

// Return number of x bins
int Histogram3D::nXBins() const { return nXBins_; }

// Return minimum value for y data (hard left-edge of first bin)
double Histogram3D::yMinimum() const { return yMinimum_; }

// Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram3D::yMaximum() const { return yMaximum_; }

// Return y bin width
double Histogram3D::yBinWidth() const { return yBinWidth_; }

// Return number of y bins
int Histogram3D::nYBins() const { return nYBins_; }

// Bin specified value, returning success
bool Histogram3D::bin(double x, double y, double z)
{
    // Calculate target bin along x
    auto xBin = (x - xMinimum_) / xBinWidth_;
    if ((xBin < 0) || (xBin >= nXBins_))
    {
        ++nMissed_;
        return false;
    }

    // Calculate target bin along y
    auto yBin = (y - yMinimum_) / yBinWidth_;
    if ((yBin < 0) || (yBin >= nYBins_))
    {
        ++nMissed_;
        return false;
    }

    // Calculate target bin along z
    auto zBin = (z - zMinimum_) / zBinWidth_;
    if ((zBin < 0) || (zBin >= nZBins_))
    {
        ++nMissed_;
        return false;
    }

    ++bins_.at(xBin, yBin, zBin);
    ++nBinned_;

    return true;
}

// Bin specified value (as Vec3), returning success
bool Histogram3D::bin(Vec3<double> v) { return bin(v.x, v.y, v.z); }

// Return number of values binned over all bins
long int Histogram3D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void Histogram3D::accumulate()
{
    for (int x = 0; x < nXBins_; ++x)
    {
        for (int y = 0; y < nYBins_; ++y)
        {
            for (int z = 0; z < nZBins_; ++z)
                averages_.at(x, y, z) += double(bins_.at(x, y, z));
        }
    }

    // Update accumulated data
    updateAccumulatedData();
}

// Return Array of x centre-bin values
const Array<double> &Histogram3D::xBinCentres() const { return xBinCentres_; }

// Return Array of y centre-bin values
const Array<double> &Histogram3D::yBinCentres() const { return yBinCentres_; }

// Return histogram data
Array3D<long int> &Histogram3D::bins() { return bins_; }

// Add source histogram data into local array
void Histogram3D::add(Histogram3D &other, int factor)
{
    if ((nXBins_ != other.nXBins_) || (nYBins_ != other.nYBins_))
    {
        Messenger::print("BAD_USAGE - Can't add Histogram3D data since arrays are not the same size ({}x{} vs {}x{}).\n",
                         nXBins_, nYBins_, other.nXBins_, other.nYBins_);
        return;
    }

    for (int x = 0; x < nXBins_; ++x)
    {
        for (int y = 0; y < nYBins_; ++y)
        {
            for (int z = 0; z < nZBins_; ++z)
                bins_.at(x, y, z) += other.bins_.at(x, y, z) * factor;
        }
    }
}

// Return accumulated (averaged) data
const Data3D &Histogram3D::accumulatedData() const { return accumulatedData_; }

/*
 * Operators
 */

void Histogram3D::operator=(const Histogram3D &source)
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
std::string_view Histogram3D::itemClassName() { return "Histogram3D"; }

// Read data through specified LineParser
bool Histogram3D::read(LineParser &parser, CoreData &coreData)
{
    clear();

    if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;
    setObjectTag(parser.line());

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argd(0), parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4), parser.argd(5), parser.argd(6),
               parser.argd(7), parser.argd(8));

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    for (int x = 0; x < nXBins_; ++x)
    {
        for (int y = 0; y < nYBins_; ++y)
        {
            for (int z = 0; z < nZBins_; ++z)
                if (!averages_.at(x, y, z).read(parser, coreData))
                    return false;
        }
    }

    return true;
}

// Write data through specified LineParser
bool Histogram3D::write(LineParser &parser)
{
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{} {} {} {} {} {} {} {} {}\n", xMinimum_, xMaximum_, xBinWidth_, yMinimum_, yMaximum_, yBinWidth_,
                           zMinimum_, zMaximum_, zBinWidth_))
        return false;
    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;
    for (int x = 0; x < nXBins_; ++x)
    {
        for (int y = 0; y < nYBins_; ++y)
        {
            for (int z = 0; z < nZBins_; ++z)
                if (!averages_.at(x, y, z).write(parser))
                    return false;
        }
    }

    return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram3D::allSum(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.allSum(bins_.linearArray(), bins_.linearArraySize()))
        return false;
#endif

    return true;
}

// Broadcast data
bool Histogram3D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    // Range data
    if (!procPool.broadcast(xMinimum_, root))
        return false;
    if (!procPool.broadcast(xMaximum_, root))
        return false;
    if (!procPool.broadcast(xBinWidth_, root))
        return false;
    if (!procPool.broadcast(nXBins_, root))
        return false;
    if (!procPool.broadcast(yMinimum_, root))
        return false;
    if (!procPool.broadcast(yMaximum_, root))
        return false;
    if (!procPool.broadcast(yBinWidth_, root))
        return false;
    if (!procPool.broadcast(nYBins_, root))
        return false;
    if (!procPool.broadcast(zMinimum_, root))
        return false;
    if (!procPool.broadcast(zMaximum_, root))
        return false;
    if (!procPool.broadcast(zBinWidth_, root))
        return false;
    if (!procPool.broadcast(nZBins_, root))
        return false;

    // Data
    if (!procPool.broadcast(nBinned_, root))
        return false;
    if (!procPool.broadcast(nMissed_, root))
        return false;
    if (!procPool.broadcast(xBinCentres_, root))
        return false;
    if (!procPool.broadcast(yBinCentres_, root))
        return false;
    if (!procPool.broadcast(zBinCentres_, root))
        return false;
    if (!procPool.broadcast(bins_.linearArray(), bins_.linearArraySize(), root))
        return false;
    SampledDouble *avgs = averages_.linearArray();
    for (int n = 0; n < averages_.linearArraySize(); ++n)
        if (!avgs[n].broadcast(procPool, root, coreData))
            return false;
#endif
    return true;
}

// Check item equality
bool Histogram3D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    // Check number of items in arrays first
    if (!procPool.equality(xMinimum_))
        return Messenger::error("Histogram3D minimum x value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xMinimum_);
    if (!procPool.equality(xMaximum_))
        return Messenger::error("Histogram3D maximum x value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xMaximum_);
    if (!procPool.equality(xBinWidth_))
        return Messenger::error("Histogram3D bin x width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xBinWidth_);
    if (!procPool.equality(nXBins_))
        return Messenger::error("Histogram3D number of x bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nXBins_);
    if (!procPool.equality(xBinCentres_))
        return Messenger::error("Histogram3D x bin centre values not equivalent.\n");
    if (!procPool.equality(yMinimum_))
        return Messenger::error("Histogram3D minimum y value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yMinimum_);
    if (!procPool.equality(yMaximum_))
        return Messenger::error("Histogram3D maximum y value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yMaximum_);
    if (!procPool.equality(yBinWidth_))
        return Messenger::error("Histogram3D bin y width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yBinWidth_);
    if (!procPool.equality(nYBins_))
        return Messenger::error("Histogram3D number of y bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nYBins_);
    if (!procPool.equality(yBinCentres_))
        return Messenger::error("Histogram3D y bin centre values not equivalent.\n");
    if (!procPool.equality(zMinimum_))
        return Messenger::error("Histogram3D minimum z value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                zMinimum_);
    if (!procPool.equality(zMaximum_))
        return Messenger::error("Histogram3D maximum z value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                zMaximum_);
    if (!procPool.equality(zBinWidth_))
        return Messenger::error("Histogram3D bin z width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                zBinWidth_);
    if (!procPool.equality(nZBins_))
        return Messenger::error("Histogram3D number of z bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nZBins_);
    if (!procPool.equality(zBinCentres_))
        return Messenger::error("Histogram3D z bin centre values not equivalent.\n");
    if (!procPool.equality(bins_.linearArray(), bins_.linearArraySize()))
        return Messenger::error("Histogram3D bin values not equivalent.\n");
    if (!procPool.equality(nBinned_))
        return Messenger::error("Histogram3D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    if (!procPool.equality(nMissed_))
        return Messenger::error("Histogram3D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    SampledDouble *avgs = averages_.linearArray();
    for (int n = 0; n < averages_.linearArraySize(); ++n)
        if (!avgs[n].equality(procPool))
            return Messenger::error("Histogram3D average values not equivalent.\n");
#endif
    return true;
}
