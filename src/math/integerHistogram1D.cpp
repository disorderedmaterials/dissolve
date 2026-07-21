// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/integerHistogram1D.h"
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

// Return average map
const std::map<int, SampledDouble> &IntegerHistogram1D::averages() const { return averages_; }

// Return accumulated (averaged) data
const Data1D &IntegerHistogram1D::accumulatedData() const { return accumulatedData_; }

/*
 * Serialisation
 */

// Express as a serialisable value
void IntegerHistogram1D::serialise(std::string tag, SerialisedValue &target) const
{
    using namespace Serialisable;
    target[tag] = {{"zeroCounter", ser(zeroCounter_)}, {"nBinned", nBinned_}, {"nMissed", nMissed_}};

    if (minimum_)
        target[tag]["minimum"] = *minimum_;
    if (maximum_)
        target[tag]["maximum"] = *maximum_;

    map(averages_, "averages", target);
}

// Read values from a serialisable value
void IntegerHistogram1D::deserialise(const SerialisedValue &node)
{
    using namespace Deserialisable;
    clear();

    Deserialisable::getIfPresent<int>(node, "minimum", minimum_);
    Deserialisable::getIfPresent<int>(node, "maximum", maximum_);

    nBinned_ = de<long>(node.at("nBinned"));
    nMissed_ = de<long>(node.at("nMissed"));
    zeroCounter_ = de<SampledDouble>(node.at("nMissed"));

    Deserialisable::map(node, "averages",
                        [&](const auto &key, const auto &value) { averages_[std::stoi(key)].deserialise(value); });

    updateAccumulatedData();
}
