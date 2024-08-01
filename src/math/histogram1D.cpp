// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/histogram1D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "templates/algorithms.h"
#include <cassert>

Histogram1D::Histogram1D()
{
    accumulatedData_.addErrors();

    clear();
}

Histogram1D::Histogram1D(const Histogram1D &source) { (*this) = source; }

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
    accumulatedData_.initialise(bins_.size(), true);

    // Store bin centres and accumulated averages in the object
    for (auto n = 0; n < bins_.size(); ++n)
    {
        accumulatedData_.xAxis(n) = binCentres_[n];
        accumulatedData_.value(n) = averages_[n];
        accumulatedData_.error(n) = averages_[n].stDev();
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
    bins_.clear();
    bins_.resize(nBins_);
    averages_.clear();
    averages_.resize(nBins_);
}

// Zero histogram bins
void Histogram1D::zeroBins()
{
    std::fill(bins_.begin(), bins_.end(), 0);
    nBinned_ = 0;
    nMissed_ = 0;
}

// Set up supplied axis
void Histogram1D::setUpAxis(double axisMin, double &axisMax, double binWidth, int &nBins, std::vector<double> &binCentres)
{
    // Min, max, and bin width should be set to requested values initially
    // We will clamp the maximum to the nearest bin boundary (not less than the supplied axisMax)
    double range = axisMax - axisMin;
    nBins = int(range / binWidth);
    if ((axisMin + nBins * binWidth) < axisMax)
    {
        ++nBins;
        axisMax = axisMin + nBins * binWidth;
    }

    // Create centre-bin array
    binCentres.clear();
    binCentres.resize(nBins);
    double centre = axisMin + binWidth * 0.5;
    for (auto n = 0; n < nBins; ++n, centre += binWidth)
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
    if (nBins_==1) { bin(0); }

    auto bin = int((x - minimum_) / binWidth_);

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
    for (auto n = 0; n < nBins_; ++n)
        averages_[n] += double(bins_[n]);

    // Update accumulated data
    updateAccumulatedData();
}

// Return Array of x centre-bin values
const std::vector<double> &Histogram1D::binCentres() const { return binCentres_; }

// Return histogram data
std::vector<long int> &Histogram1D::bins() { return bins_; }
const std::vector<long int> &Histogram1D::bins() const { return bins_; }

// Add source histogram data into local array
void Histogram1D::add(Histogram1D &other, int factor)
{
    if (nBins_ != other.nBins_)
    {
        Messenger::print("BAD_USAGE - Can't add Histogram1D data since arrays are not the same size ({} vs {}).\n", nBins_,
                         other.nBins_);
        return;
    }
    for (auto n = 0; n < nBins_; ++n)
        bins_[n] += other.bins_[n] * factor;

    nBinned_ += other.nBinned_;
    nMissed_ += other.nMissed_;
}

// Return current data
Data1D Histogram1D::data() const
{
    Data1D result = accumulatedData_;
    std::copy(bins_.begin(), bins_.end(), result.values().begin());
    return result;
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

Histogram1D Histogram1D::operator+(const Histogram1D &other) const
{
    assert(nBins_ == other.nBins_);

    Histogram1D ret = *this;

    std::transform(other.bins_.cbegin(), other.bins_.cend(), ret.bins_.cbegin(), ret.bins_.begin(), std::plus<>());

    ret.nBinned_ = this->nBinned_ + other.nBinned_;
    ret.nMissed_ = this->nMissed_ + other.nMissed_;
    ret.nBins_ = this->nBins_;

    return ret;
}

/*
 * Serialisation
 */

// Read data through specified LineParser
bool Histogram1D::deserialise(LineParser &parser)
{
    clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argd(0), parser.argd(1), parser.argd(2));

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    for (auto n = 0; n < nBins_; ++n)
        if (!averages_[n].deserialise(parser))
            return false;

    return true;
}

// Write data through specified LineParser
bool Histogram1D::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{} {} {}\n", minimum_, maximum_, binWidth_))
        return false;
    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;
    for (auto n = 0; n < nBins_; ++n)
        if (!averages_.at(n).serialise(parser))
            return false;

    return true;
}

/*
 * Parallel Comms
 */

// Sum histogram data onto all processes
bool Histogram1D::allSum(const ProcessPool &procPool, OptionalReferenceWrapper<Timer> commsTimer)
{
#ifdef PARALLEL
    if (!procPool.allSum(bins_.data(), nBins_, ProcessPool::PoolProcessesCommunicator, commsTimer))
        return false;
#endif
    return true;
}
