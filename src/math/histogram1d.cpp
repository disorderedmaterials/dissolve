/*
    *** 1-Dimensional Histogram
    *** src/math/histogram1d.cpp
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

#include "math/histogram1d.h"
#include "base/lineparser.h"
#include "base/messenger.h"

// Static Members (ObjectStore)
template <class Histogram1D> RefDataList<Histogram1D, int> ObjectStore<Histogram1D>::objects_;
template <class Histogram1D> int ObjectStore<Histogram1D>::objectCount_ = 0;
template <class Histogram1D> int ObjectStore<Histogram1D>::objectType_ = ObjectInfo::Histogram1DObject;
template <class Histogram1D> std::string_view ObjectStore<Histogram1D>::objectTypeName_ = "Histogram1D";

Histogram1D::Histogram1D() : ListItem<Histogram1D>(), ObjectStore<Histogram1D>(this)
{
    accumulatedData_.addErrors();

    clear();
}

Histogram1D::~Histogram1D() {}

Histogram1D::Histogram1D(const Histogram1D &source) : ObjectStore<Histogram1D>(this) { (*this) = source; }

// Clear Data
void Histogram1D::clear()
{
    minimum_ = 0.0;
    maximum_ = 0.0;
    binWidth_ = 0.0;
    nBins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
    bins_.clear();
    binCentres_.clear();
    averages_.clear();
}

/*
 * Data
 */

// Update accumulated data
void Histogram1D::updateAccumulatedData()
{
    // Set up arrays
    accumulatedData_.initialise(bins_.nItems(), true);

    // Store bin centres and accumulated averages in the object
    for (int n = 0; n < bins_.nItems(); ++n)
    {
        accumulatedData_.xAxis(n) = binCentres_[n];
        accumulatedData_.value(n) = averages_.constAt(n);
        accumulatedData_.error(n) = averages_.constAt(n).stDev();
    }
}

// Initialise with specified bin range
void Histogram1D::initialise(double xMin, double xMax, double binWidth)
{
    clear();

    // Store axis limits and binWidth, and then clamp as necessary
    minimum_ = xMin;
    maximum_ = xMax;
    binWidth_ = binWidth;
    setUpAxis(minimum_, maximum_, binWidth_, nBins_, binCentres_);

    // Create the arrays
    bins_.initialise(nBins_);
    averages_.initialise(nBins_);
}

// Zero histogram bins
void Histogram1D::zeroBins()
{
    bins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
}

// Set up supplied axis
void Histogram1D::setUpAxis(double axisMin, double &axisMax, double binWidth, int &nBins, Array<double> &binCentres)
{
    // Min, max, and bin width should be set to requested values initially
    // We will clamp the maximum to the nearest bin boundary (not less than the supplied axisMax)
    double range = axisMax - axisMin;
    nBins = range / binWidth;
    if ((axisMin + nBins * binWidth) < axisMax)
    {
        ++nBins;
        axisMax = axisMin + nBins * binWidth;
    }

    // Create centre-bin array
    binCentres.initialise(nBins);
    double centre = axisMin + binWidth * 0.5;
    for (int n = 0; n < nBins; ++n, centre += binWidth)
        binCentres[n] = centre;
}

// Return minimum value for data (hard left-edge of first bin)
double Histogram1D::minimum() const { return minimum_; }

// Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram1D::maximum() const { return maximum_; }

// Return bin width
double Histogram1D::binWidth() const { return binWidth_; }

// Return number of bins
int Histogram1D::nBins() const { return nBins_; }

// Bin specified value, returning success
bool Histogram1D::bin(double x)
{
    // Calculate target bin
    auto bin = (x - minimum_) / binWidth_;

    // Check bin range
    if ((bin < 0) || (bin >= nBins_))
    {
        ++nMissed_;
        return false;
    }

    ++bins_[bin];
    ++nBinned_;

    return true;
}

// Return number of values binned over all bins
long int Histogram1D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void Histogram1D::accumulate()
{
    for (int n = 0; n < nBins_; ++n)
        averages_[n] += double(bins_[n]);

    // Update accumulated data
    updateAccumulatedData();
}

// Return Array of x centre-bin values
const Array<double> &Histogram1D::binCentres() const { return binCentres_; }

// Return histogram data
Array<long int> &Histogram1D::bins() { return bins_; }

// Add source histogram data into local array
void Histogram1D::add(Histogram1D &other, int factor)
{
    if (nBins_ != other.nBins_)
    {
        Messenger::print("BAD_USAGE - Can't add Histogram1D data since arrays are not the same size ({} vs {}).\n", nBins_,
                         other.nBins_);
        return;
    }
    for (int n = 0; n < nBins_; ++n)
        bins_[n] += other.bins_[n] * factor;
}

// Return accumulated (averaged) data
const Data1D &Histogram1D::accumulatedData() const { return accumulatedData_; }

/*
 * Operators
 */

void Histogram1D::operator=(const Histogram1D &source)
{
    minimum_ = source.minimum_;
    maximum_ = source.maximum_;
    nBins_ = source.nBins_;
    binWidth_ = source.binWidth_;
    nBinned_ = source.nBinned_;
    nMissed_ = source.nMissed_;
    bins_ = source.bins_;
    binCentres_ = source.binCentres_;
    averages_ = source.averages_;
}

/*
 * GenericItemBase Implementations
 */

// Return class name
std::string_view Histogram1D::itemClassName() { return "Histogram1D"; }

// Read data through specified LineParser
bool Histogram1D::read(LineParser &parser, CoreData &coreData)
{
    clear();

    if (parser.readNextLine(LineParser::Defaults) != LineParser::Success)
        return false;
    setObjectTag(parser.line());

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argd(0), parser.argd(1), parser.argd(2));

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    for (int n = 0; n < nBins_; ++n)
        if (!averages_[n].read(parser, coreData))
            return false;

    return true;
}

// Write data through specified LineParser
bool Histogram1D::write(LineParser &parser)
{
    if (!parser.writeLineF("{}\n", objectTag()))
        return false;
    if (!parser.writeLineF("{} {} {}\n", minimum_, maximum_, binWidth_))
        return false;
    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;
    for (int n = 0; n < nBins_; ++n)
        if (!averages_[n].write(parser))
            return false;

    return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram1D::allSum(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.allSum(bins_, nBins_))
        return false;
#endif
    return true;
}

// Broadcast data
bool Histogram1D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
#ifdef PARALLEL
    // Range data
    if (!procPool.broadcast(minimum_, root))
        return false;
    if (!procPool.broadcast(maximum_, root))
        return false;
    if (!procPool.broadcast(binWidth_, root))
        return false;
    if (!procPool.broadcast(nBins_, root))
        return false;

    // Data
    if (!procPool.broadcast(nBinned_, root))
        return false;
    if (!procPool.broadcast(nMissed_, root))
        return false;
    if (!procPool.broadcast(binCentres_, root))
        return false;
    if (!procPool.broadcast(bins_, root))
        return false;
    for (int n = 0; n < averages_.nItems(); ++n)
        if (!averages_[n].broadcast(procPool, root, coreData))
            return false;
#endif
    return true;
}

// Check item equality
bool Histogram1D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    // Check number of items in arrays first
    if (!procPool.equality(minimum_))
        return Messenger::error("Histogram1D minimum value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                minimum_);
    if (!procPool.equality(maximum_))
        return Messenger::error("Histogram1D maximum value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                maximum_);
    if (!procPool.equality(binWidth_))
        return Messenger::error("Histogram1D bin width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                binWidth_);
    if (!procPool.equality(nBins_))
        return Messenger::error("Histogram1D number of bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nBins_);
    if (!procPool.equality(binCentres_))
        return Messenger::error("Histogram1D bin centre values not equivalent.\n");
    if (!procPool.equality(bins_))
        return Messenger::error("Histogram1D bin values not equivalent.\n");
    if (!procPool.equality(nBinned_))
        return Messenger::error("Histogram1D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    if (!procPool.equality(nMissed_))
        return Messenger::error("Histogram1D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    for (int n = 0; n < averages_.nItems(); ++n)
        if (!averages_[n].equality(procPool))
            return Messenger::error("Histogram1D average values not equivalent.\n");
#endif
    return true;
}
