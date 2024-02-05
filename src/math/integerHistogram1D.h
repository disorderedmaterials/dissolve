// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "keywords/optionalInt.h"
#include "math/data1D.h"
#include "math/sampledDouble.h"
#include <map>

// Pure Integer Histogram
class IntegerHistogram1D
{
    public:
    IntegerHistogram1D();
    ~IntegerHistogram1D() = default;
    IntegerHistogram1D(const IntegerHistogram1D &source);
    // Clear data
    void clear();

    /*
     * Histogram Data
     */
    private:
    // Minimum value for binning
    std::optional<int> minimum_{0};
    // Maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    std::optional<int> maximum_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data1D accumulatedData_;
    // Map of histogram bin values to instantaneous counts and accumulated averages
    std::map<int, long int> raw_;
    std::map<int, SampledDouble> averages_;
    // Zero-valued sampling counter
    SampledDouble zeroCounter_;

    private:
    // Update accumulated data
    void updateAccumulatedData();
    // Create display data object covering extents of current bins
    std::pair<Data1D, int> createDisplayData() const;

    public:
    // Initialise with specified bin range
    void initialise(std::optional<int> xMin = {}, std::optional<int> xMax = {});
    // Zero histogram bins
    void zeroBins();
    // Return minimum value for data
    std::optional<int> minimum() const;
    // Return maximum value for data
    std::optional<int> maximum() const;
    // Bin specified value, returning success
    bool bin(int x, int count = 1);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return current data
    Data1D data() const;
    // Return averages map
    const std::map<int, SampledDouble> &averages() const;
    // Return accumulated (averaged) data
    const Data1D &accumulatedData() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
