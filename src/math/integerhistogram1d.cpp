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
    bins_.clear();
}

/*
 * Data
 */

// Update accumulated data
void IntegerHistogram1D::updateAccumulatedData()
{
    accumulatedData_ = createDisplayData();
    auto minBin = DissolveMath::nInt(accumulatedData_.xAxis().front());
    // Poke bin values and errors into array
    for (const auto &[key, value] : bins_)
    {
        auto n = key - minBin;
        accumulatedData_.value(n) = value.second.value();
        accumulatedData_.error(n) = value.second.stDev();
    }
}

// Create dsiplay data
const Data1D IntegerHistogram1D::createDisplayData()
{
    Data1D data;
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
    data.initialise(expectedNBins, true);
    auto x = *expectedMinimum;
    for (auto n = 0; n < expectedNBins; n++)
        data.xAxis(n) = x++;
    return data;
}

// Initialise with specified bin range
void IntegerHistogram1D::initialise(std::optional<int> xMin, std::optional<int> xMax)
{
    clear();

    minimum_ = xMin;
    maximum_ = xMax;

    bins_.clear();
}

// Zero histogram bins
void IntegerHistogram1D::zeroBins()
{
    bins_.clear();
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
        result.values()[(key)] = value.first;
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

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;

    auto nBins = parser.argli(0);

    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    for (int n; n <= nBins; n++)
    {
        bins_[parser.argli(0)].first = parser.argli(1);
        bins_[parser.argli(0)].second.deserialise(parser);
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

    if (!parser.writeLineF("{} \n", bins_.size()))
        return false;

    for (auto &[key, value] : bins_)
    {
        auto &[count, average] = value;
        if (!parser.writeLineF("{} {} \n", key, count))
            return false;
        if (!average.serialise(parser))
            return false;
    }

    return true;
}
