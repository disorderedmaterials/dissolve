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
    // Get limiting key values
    std::optional<int> expectedMinimum = bins_.empty() ? std::nullopt : std::optional<int>(bins_.begin()->first);
    if (minimum_)
        expectedMinimum = expectedMinimum ? std::min(*minimum_, *expectedMinimum) : *minimum_;

    std::optional<int> expectedMaximum = bins_.empty() ? std::nullopt : std::optional<int>(std::prev(bins_.end())->first);
    if (maximum_)
        expectedMaximum = expectedMaximum ? std::max(*maximum_, *expectedMaximum) : *maximum_;

    // Need to check on null values - set to zeroes if both null, or if one is null set it to match the other
    if (!expectedMinimum && !expectedMaximum)
    {
        expectedMinimum = 0;
        expectedMaximum = 0;
    }
    else if (!expectedMinimum)
        expectedMinimum = expectedMaximum;
    else if (!expectedMaximum)
        expectedMaximum = expectedMinimum;

    auto expectedNBins = (*expectedMaximum - *expectedMinimum) + 1;

    // Set up data
    accumulatedData_.initialise(expectedNBins, true);
    auto x = *expectedMinimum;
    for (auto n = 0; n < expectedNBins; n++)
        accumulatedData_.xAxis(n) = x++;

    // Poke bin values and errors into array
    for (const auto &[key, value] : bins_)
    {
        auto n = key - *expectedMinimum;
        accumulatedData_.value(n) = value.second.value();
        accumulatedData_.error(n) = value.second.stDev();
    }
}

// Initialise with specified bin range
void IntegerHistogram1D::initialise(std::optional<int> xMin, std::optional<int> xMax)
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

// Return minimum value for data (hard left-edge of first bin)
std::optional<int> IntegerHistogram1D::minimum() const { return minimum_; }

// Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
std::optional<int> IntegerHistogram1D::maximum() const { return maximum_; }

// Bin specified value, returning success
bool IntegerHistogram1D::bin(int x)
{
    if ((minimum_ && x < *minimum_) || (maximum_ && x > *maximum_))
    {
        nMissed_++;
        return false;
    }

    bins_[x].first++;

    return true;
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

// Return histogram data
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
    initialise(parser.argd(0), parser.argd(1));

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
