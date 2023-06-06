// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/integerhistogram1d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "templates/algorithms.h"
#include <cassert>

IntegerHistogram1D::IntegerHistogram1D()
{
    accumulatedData_.addErrors();

    clear();
}

IntegerHistogram1D::IntegerHistogram1D(const IntegerHistogram1D &source) { (*this) = source; }

// Clear Data
void IntegerHistogram1D::clear()
{
    minimum_ = 0;
    maximum_ = 0;
    nBinned_ = 0;
    nMissed_ = 0;
    bins_.clear();
}

/*
 * Data
 */

// Update accumulated data
void IntegerHistogram1D::updateAccumulatedData()
{
    // Set up arrays
    accumulatedData_.initialise(bins_.size(), true);

    // Store bin centres and accumulated averages in the object
    for (auto n = 0; n < bins_.size(); ++n)
    {
        accumulatedData_.xAxis(n) = n;
    }
}

// Initialise with specified bin range
void IntegerHistogram1D::initialise(int xMin, int xMax, int binWidth)
{
    clear();

    // Store axis limits and binWidth, and then clamp as necessary
    minimum_ = xMin;
    maximum_ = xMax;

    // Create the arrays
    bins_.clear();
}

// Zero histogram bins
void IntegerHistogram1D::zeroBins()
{
    bins_.clear();
    nBinned_ = 0;
    nMissed_ = 0;
}

// Set up supplied axis
void IntegerHistogram1D::setUpAxis(int axisMin, int &axisMax, int binWidth, int &nBins, std::vector<int> &binCentres)
{
    // Min, max, and bin width should be set to requested values initially
    // We will clamp the maximum to the nearest bin boundary (not less than the supplied axisMax)
    int range = axisMax - axisMin;
    nBins = range;
    if ((axisMin + nBins * binWidth) < axisMax)
    {
        ++nBins;
        axisMax = axisMin + nBins * binWidth;
    }
}

// Return minimum value for data (hard left-edge of first bin)
std::optional<int> IntegerHistogram1D::minimum() const { return minimum_; }

// Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
std::optional<int> IntegerHistogram1D::maximum() const { return maximum_; }
// Bin specified value, returning success
bool IntegerHistogram1D::bin(int x)
{
    if (x <= maximum_ && x >= minimum_)
        bins_[x].first++;
    else
        nMissed_++;
}

// Return number of values binned over all bins
long int IntegerHistogram1D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void IntegerHistogram1D::accumulate()
{
    for (auto &[key, value] : bins_)
        value.second += (SampledDouble)value.first;

    // Update accumulated data
    updateAccumulatedData();
}

//  Return histogram data
std::map<int, std::pair<long int, SampledDouble>> &IntegerHistogram1D::bins() { return bins_; }
const std::map<int, std::pair<long int, SampledDouble>> &IntegerHistogram1D::bins() const { return bins_; }

// Add source histogram data into local array
void IntegerHistogram1D::add(IntegerHistogram1D &other, int factor)
{
    if (bins_.size() != other.bins_.size())
    {
        Messenger::print("BAD_USAGE - Can't add IntegerHistogram1D data since arrays are not the same size ({} vs {}).\n",
                         bins_.size(), other.bins_.size());
        return;
    }
    for (auto &[key, value] : bins_)

        bins_[value.first].first += other.bins_[value.first].first * factor;
}
// Return current data
Data1D IntegerHistogram1D::data() const
{
    Data1D result = accumulatedData_;
    for (auto &[key, value] : bins_)
    {
        result.values().push_back(value.first);
    }
    /*std::copy(bins_.begin(), bins_.end(), result.values().begin());*/
    return result;
}

// Return accumulated (averaged) data
const Data1D &IntegerHistogram1D::accumulatedData() const { return accumulatedData_; }

/*
 * Operators
 */

// void IntegerHistogram1D::operator=(const IntegerHistogram1D &source)
//{
//     minimum_ = source.minimum_;
//     maximum_ = source.maximum_;
//     nBinned_ = source.nBinned_;
//     nMissed_ = source.nMissed_;
//     bins_ = source.bins_;
//     averages_ = source.averages_;
// }

// IntegerHistogram1D IntegerHistogram1D::operator+(const IntegerHistogram1D &other) const
//{
//     assert(bins_.size() == other.bins_.size());
//
//     IntegerHistogram1D ret = *this;
//
//     std::transform(other.bins_.cbegin(), other.bins_.cend(), ret.bins_.cbegin(), ret.bins_.begin(), std::plus<>());
//
//     ret.nBinned_ = this->nBinned_ + other.nBinned_;
//     ret.nMissed_ = this->nMissed_ + other.nMissed_;
//
//     return ret;
// }

/*
 * Serialisation
 */

// Read data through specified LineParser
bool IntegerHistogram1D::deserialise(LineParser &parser)
{
    clear();

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    initialise(parser.argd(0), parser.argd(1), parser.argd(2));

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    return true;
}

// Write data through specified LineParser
bool IntegerHistogram1D::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{} {} {} {}\n", DissolveSys::btoa(minimum_.has_value()), minimum_.value_or(0),
                           DissolveSys::btoa(maximum_.has_value()), maximum_.value_or(0)))
        return false;
    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;

    return true;
}
