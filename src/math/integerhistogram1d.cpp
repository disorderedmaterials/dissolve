// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "math/integerhistogram1d.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include "math/mathfunc.h"
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
    raw_.clear();
    averages_.clear();
}

/*
 * Data
 */

// Update accumulated data
void IntegerHistogram1D::updateAccumulatedData()
{
    auto average = createDisplayData();
    accumulatedData_ = average.first;
    auto minBin = average.second;
    // Poke bin values and errors into array
    for (const auto &[key, value] : averages_)
    {
        auto n = key - minBin;
        accumulatedData_.value(n) = value.value();
        accumulatedData_.error(n) = value.stDev();
    }
}

// Create dsiplay data object covering extents of current bins
const std::pair<Data1D, int> IntegerHistogram1D::createDisplayData()
{

    // Get limiting key values
    std::optional<int> expectedMinimum = raw_.empty() ? std::nullopt : std::optional<int>(raw_.begin()->first);
    if (minimum_)
        expectedMinimum = expectedMinimum ? std::min(*minimum_, *expectedMinimum) : *minimum_;

    std::optional<int> expectedMaximum = raw_.empty() ? std::nullopt : std::optional<int>(std::prev(raw_.end())->first);
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
    Data1D data;
    data.initialise(expectedNBins, true);
    auto x = *expectedMinimum;
    for (auto n = 0; n < expectedNBins; ++n)
        data.xAxis(n) = x++;

    return std::make_pair(data, expectedMinimum.value());
}

// Initialise with specified bin range
void IntegerHistogram1D::initialise(std::optional<int> xMin, std::optional<int> xMax)
{
    clear();

    minimum_ = xMin;
    maximum_ = xMax;
}

// Zero histogram bins
void IntegerHistogram1D::zeroBins()
{
    raw_.clear();
    nBinned_ = 0;
    nMissed_ = 0;
}

// Return minimum value for data
std::optional<int> IntegerHistogram1D::minimum() const { return minimum_; }

// Return maximum value for data
std::optional<int> IntegerHistogram1D::maximum() const { return maximum_; }

// Bin specified value, returning success
bool IntegerHistogram1D::bin(int x)
{
    if ((minimum_ && x < *minimum_) || (maximum_ && x > *maximum_))
    {
        nMissed_++;
        return false;
    }

    raw_[x]++;
    ++nBinned_;
}

// Return number of values binned over all bins
long int IntegerHistogram1D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void IntegerHistogram1D::accumulate()
{
    for (auto &[key, value] : raw_)
        averages_[key] += (double)raw_[key];

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

// Return current data
Data1D IntegerHistogram1D::data() const
{
    Data1D result = accumulatedData_;
    for (auto &[key, value] : raw_)
    {
        result.values()[key] = value;
    }
    return result;
}

// Return accumulated (averaged) data
const Data1D &IntegerHistogram1D::accumulatedData() const { return accumulatedData_; }

/*
<<<<<<< HEAD
=======
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
>>>>>>> abf1b949c (added in de/serialise and averages)
 * Serialisation
 */

// Read data through specified LineParser
bool IntegerHistogram1D::deserialise(LineParser &parser)
{
    clear();
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    std::optional<int> minLim = parser.argb(0) ? std::optional<int>(parser.argi(1)) : std::nullopt;
    std::optional<int> maxLim = parser.argb(2) ? std::optional<int>(parser.argi(3)) : std::nullopt;
    initialise(minLim, maxLim);

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    nBinned_ = parser.argli(0);
    nMissed_ = parser.argli(1);

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    auto nBins = parser.argli(0);

    for (int n = 0; n <= nBins; ++n)

    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        auto bin = parser.argi(0);
        raw_[bin] = parser.argli(1);
        averages_[bin].deserialise(parser);
    }
    return true;
}

// Write data through specified LineParser
bool IntegerHistogram1D::serialise(LineParser &parser) const
{
    if (!parser.writeLineF("{} {} {} {}\n", DissolveSys::btoa(minimum_.has_value()), minimum_.value_or(0),
                           DissolveSys::btoa(maximum_.has_value()), maximum_.value_or(0)))
        return false;
    if (!parser.writeLineF("{}  {} \n", nBinned_, nMissed_))
        return false;

    if (!parser.writeLineF("{} \n", raw_.size()))
        return false;

    for (auto &[key, value] : raw_)
    {
        if (!parser.writeLineF("{} {} \n", key, value))
            return false;
        if (!averages_.at(key).serialise(parser))
            return false;
    }

    return true;
}