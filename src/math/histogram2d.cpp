// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/histogram2d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "math/histogram1d.h"

Histogram2D::Histogram2D() : ListItem<Histogram2D>()
{
    accumulatedData_.addErrors();

    clear();
}

Histogram2D::~Histogram2D() {}

Histogram2D::Histogram2D(const Histogram2D &source) { (*this) = source; }

// Clear Data
void Histogram2D::clear()
{
    xMinimum_ = 0.0;
    xMaximum_ = 0.0;
    xBinWidth_ = 0.0;
    nXBins_ = 0;
    yMinimum_ = 0.0;
    yMaximum_ = 0.0;
    yBinWidth_ = 0.0;
    nYBins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
    bins_.clear();
    xBinCentres_.clear();
    yBinCentres_.clear();
    averages_.clear();
}

/*
 * Data
 */

// Initialise with specified bin range
void Histogram2D::initialise(double xMin, double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth)
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

    // Create the main bins array
    bins_.initialise(nXBins_, nYBins_);
    averages_.initialise(nXBins_, nYBins_);

    // Set up accumulated data array and set bin centres
    accumulatedData_.initialise(nXBins_, nYBins_, true);
    for (auto x = 0; x < nXBins_; ++x)
        accumulatedData_.xAxis(x) = xBinCentres_[x];
    for (auto y = 0; y < nYBins_; ++y)
        accumulatedData_.yAxis(y) = yBinCentres_[y];
}

// Zero histogram bins
void Histogram2D::zeroBins()
{
    bins_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
}

// Return minimum value for x data (hard left-edge of first bin)
double Histogram2D::xMinimum() const { return xMinimum_; }

// Return maximum value for x data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram2D::xMaximum() const { return xMaximum_; }

// Return x bin width
double Histogram2D::xBinWidth() const { return xBinWidth_; }

// Return number of x bins
int Histogram2D::nXBins() const { return nXBins_; }

// Return minimum value for y data (hard left-edge of first bin)
double Histogram2D::yMinimum() const { return yMinimum_; }

// Return maximum value for y data (hard right-edge of last bin, adjusted to match bin width if necessary)
double Histogram2D::yMaximum() const { return yMaximum_; }

// Return y bin width
double Histogram2D::yBinWidth() const { return yBinWidth_; }

// Return number of y bins
int Histogram2D::nYBins() const { return nYBins_; }

// Bin specified value, returning success
bool Histogram2D::bin(double x, double y)
{
    // Calculate target bin along x
    auto xBin = int((x - xMinimum_) / xBinWidth_);
    if ((xBin < 0) || (xBin >= nXBins_))
    {
        ++nMissed_;
        return false;
    }

    // Calculate target bin along y
    auto yBin = int((y - yMinimum_) / yBinWidth_);
    if ((yBin < 0) || (yBin >= nYBins_))
    {
        ++nMissed_;
        return false;
    }

    ++bins_[{xBin, yBin}];
    ++nBinned_;

    return true;
}

// Return number of values binned over all bins
long int Histogram2D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void Histogram2D::accumulate()
{
    for (auto x = 0; x < nXBins_; ++x)
    {
        for (auto y = 0; y < nYBins_; ++y)
        {
            // Update averages
            averages_[{x, y}] += double(bins_[{x, y}]);

            // Update accumulated data
            accumulatedData_.value(x, y) = averages_[{x, y}].value();
            accumulatedData_.error(x, y) = averages_[{x, y}].stDev();
        }
    }
}

// Return Array of x centre-bin values
const std::vector<double> &Histogram2D::xBinCentres() const { return xBinCentres_; }

// Return Array of y centre-bin values
const std::vector<double> &Histogram2D::yBinCentres() const { return yBinCentres_; }

// Return histogram data
Array2D<long int> &Histogram2D::bins() { return bins_; }

// Add source histogram data into local array
void Histogram2D::add(Histogram2D &other, int factor)
{
    if ((nXBins_ != other.nXBins_) || (nYBins_ != other.nYBins_))
    {
        Messenger::print("BAD_USAGE - Can't add Histogram2D data since arrays are not the same size ({}x{} vs {}x{}).\n",
                         nXBins_, nYBins_, other.nXBins_, other.nYBins_);
        return;
    }

    for (auto x = 0; x < nXBins_; ++x)
    {
        for (auto y = 0; y < nYBins_; ++y)
            bins_[{x, y}] += other.bins_[{x, y}] * factor;
    }
}

// Return accumulated (averaged) data
const Data2D &Histogram2D::accumulatedData() const { return accumulatedData_; }

/*
 * Operators
 */

void Histogram2D::operator=(const Histogram2D &source)
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
std::string_view Histogram2D::itemClassName() { return "Histogram2D"; }

// Read data through specified LineParser
bool Histogram2D::read(LineParser &parser, CoreData &coreData)
{
    clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argd(0), parser.argd(1), parser.argd(2), parser.argd(3), parser.argd(4), parser.argd(5));

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    for (auto x = 0; x < nXBins_; ++x)
    {
        for (auto y = 0; y < nYBins_; ++y)
            if (!averages_[{x, y}].read(parser, coreData))
                return false;
    }

    return true;
}

// Write data through specified LineParser
bool Histogram2D::write(LineParser &parser)
{
    if (!parser.writeLineF("{} {} {} {} {} {}\n", xMinimum_, xMaximum_, xBinWidth_, yMinimum_, yMaximum_, yBinWidth_))
        return false;
    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;
    for (auto x = 0; x < nXBins_; ++x)
    {
        for (auto y = 0; y < nYBins_; ++y)
            if (!averages_[{x, y}].write(parser))
                return false;
    }

    return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram2D::allSum(ProcessPool &procPool)
{
#ifdef PARALLEL
    if (!procPool.allSum(bins_.linearArray().data(), bins_.linearArray().size()))
        return false;
#endif

    return true;
}

// Broadcast data
bool Histogram2D::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
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

    // Data
    if (!procPool.broadcast(nBinned_, root))
        return false;
    if (!procPool.broadcast(nMissed_, root))
        return false;
    if (!procPool.broadcast(xBinCentres_, root))
        return false;
    if (!procPool.broadcast(yBinCentres_, root))
        return false;
    if (!procPool.broadcast(bins_.linearArray(), root))
        return false;
    for (auto &n : averages_)
        if (!n.broadcast(procPool, root, coreData))
            return false;
#endif
    return true;
}

// Check item equality
bool Histogram2D::equality(ProcessPool &procPool)
{
#ifdef PARALLEL
    // Check number of items in arrays first
    if (!procPool.equality(xMinimum_))
        return Messenger::error("Histogram2D minimum x value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xMinimum_);
    if (!procPool.equality(xMaximum_))
        return Messenger::error("Histogram2D maximum x value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xMaximum_);
    if (!procPool.equality(xBinWidth_))
        return Messenger::error("Histogram2D bin x width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                xBinWidth_);
    if (!procPool.equality(nXBins_))
        return Messenger::error("Histogram2D number of x bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nXBins_);
    if (!procPool.equality(xBinCentres_))
        return Messenger::error("Histogram2D x bin centre values not equivalent.\n");
    if (!procPool.equality(yMinimum_))
        return Messenger::error("Histogram2D minimum y value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yMinimum_);
    if (!procPool.equality(yMaximum_))
        return Messenger::error("Histogram2D maximum y value is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yMaximum_);
    if (!procPool.equality(yBinWidth_))
        return Messenger::error("Histogram2D bin y width is not equivalent (process {} has {:e}).\n", procPool.poolRank(),
                                yBinWidth_);
    if (!procPool.equality(nYBins_))
        return Messenger::error("Histogram2D number of y bins is not equivalent (process {} has {}).\n", procPool.poolRank(),
                                nYBins_);
    if (!procPool.equality(yBinCentres_))
        return Messenger::error("Histogram2D y bin centre values not equivalent.\n");
    if (!procPool.equality(bins_.linearArray()))
        return Messenger::error("Histogram2D bin values not equivalent.\n");
    if (!procPool.equality(nBinned_))
        return Messenger::error("Histogram2D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    if (!procPool.equality(nMissed_))
        return Messenger::error("Histogram2D nunmber of binned values is not equivalent (process {} has {}).\n",
                                procPool.poolRank(), nBinned_);
    for (auto &n : averages_)
        if (!n.equality(procPool))
            return Messenger::error("Histogram2D average values not equivalent.\n");
#endif
    return true;
}
