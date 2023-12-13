// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/integerHistogram1D.h"
#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/sysFunc.h"
#include "math/mathFunc.h"
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
    zeroCounter_.clear();
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

// Create display data object covering extents of current bins
std::pair<Data1D, int> IntegerHistogram1D::createDisplayData() const
{
    // If we haven't binned anything yet, return now
    if (raw_.empty())
        return std::make_pair(Data1D(), 0);

    // Get limiting key values
    auto expectedMinimum = minimum_ ? std::min(*minimum_, raw_.begin()->first) : raw_.begin()->first;
    auto expectedMaximum = maximum_ ? std::max(*maximum_, std::prev(raw_.end())->first) : std::prev(raw_.end())->first;
    auto expectedNBins = (expectedMaximum - expectedMinimum) + 1;

    // Set up data
    Data1D data;
    data.initialise(expectedNBins, true);
    auto x = expectedMinimum;
    for (auto n = 0; n < expectedNBins; ++n)
        data.xAxis(n) = x++;

    return std::make_pair(data, expectedMinimum);
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
    for (auto &[key, count] : raw_)
        count = 0;
    nBinned_ = 0;
    nMissed_ = 0;
}

// Return minimum value for data
std::optional<int> IntegerHistogram1D::minimum() const { return minimum_; }

// Return maximum value for data
std::optional<int> IntegerHistogram1D::maximum() const { return maximum_; }

// Bin specified value, returning success
bool IntegerHistogram1D::bin(int x, int count)
{
    if ((minimum_ && x < *minimum_) || (maximum_ && x > *maximum_))
    {
        nMissed_ += count;
        return false;
    }

    raw_[x] += count;
    nBinned_ += count;

    return true;
}

// Return number of values binned over all bins
long int IntegerHistogram1D::nBinned() const { return nBinned_; }

// Accumulate current histogram bins into averages
void IntegerHistogram1D::accumulate()
{
    for (auto &[key, value] : raw_)
        averages_.try_emplace(key, zeroCounter_).first->second += (double)raw_[key];

    // Update accumulated data
    updateAccumulatedData();

    /*
     * Accumulate zero counter - this is to keep track of the number of accumulations made
     * and allow us to "pad" the statistics of new keys added to the averages_ map
     */
    zeroCounter_ += 0.0;
}

// Return current data
Data1D IntegerHistogram1D::data() const
{
    auto displayData = createDisplayData();
    auto result = std::get<0>(displayData);
    auto xMinimum = std::get<1>(displayData);

    for (auto &[key, value] : raw_)
    {
        result.values()[key - xMinimum] = value;
    }

    return result;
}

// Return accumulated (averaged) data
const Data1D &IntegerHistogram1D::accumulatedData() const { return accumulatedData_; }

/*
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

    if (!zeroCounter_.deserialise(parser))
        return false;

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nBins = parser.argli(0);
    for (auto n = 0; n < nBins; ++n)
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

    if (!parser.writeLineF("{}  {}\n", nBinned_, nMissed_))
        return false;

    if (!zeroCounter_.serialise(parser))
        return false;

    if (!parser.writeLineF("{}\n", averages_.size()))
        return false;

    for (auto &[key, value] : averages_)
    {
        if (!parser.writeLineF("{} {}\n", key, raw_.at(key)))
            return false;
        if (!value.serialise(parser))
            return false;
    }

    return true;
}
