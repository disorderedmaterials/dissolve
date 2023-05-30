// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "keywords/optionalint.h"
#include "math/data1d.h"
#include "math/sampleddouble.h"
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
    // Accumulated averages
    std::vector<SampledDouble> averages_;
    // Number of values binned over all bins
    long int nBinned_;
    // Number of points missed (out of bin range)
    long int nMissed_;
    // Accumulated data
    Data1D accumulatedData_;
    // Map of histogram bin values to instantaneous counts and accumulated averages
    std::map<int, std::pair<long int, std::vector<SampledDouble>>> bins_;

    private:
    // Update accumulated data
    void updateAccumulatedData();

    public:
    // Initialise with specified bin range
    void initialise(int minimum, int maximum, int binWidth);
    // Zero histogram bins
    void zeroBins();
    // Set up supplied axis
    static void setUpAxis(int axisMin, int &axisMax, int binWidth, int &nBins, std::vector<int> &binCentres);
    // Return minimum value for data (hard left-edge of first bin)
    std::optional<int> minimum() const;
    // Return maximum value for data (hard right-edge of last bin, adjusted to match bin width if necessary)
    std::optional<int> maximum() const;
    // Bin specified value, returning success
    bool bin(int x);
    // Return number of values binned over all bins
    long int nBinned() const;
    // Accumulate current histogram bins into averages
    void accumulate();
    // Return histogram data
    std::map<int, std::pair<long int, std::vector<SampledDouble>>> &bins();
    const std::map<int, std::pair<long int, std::vector<SampledDouble>>> &bins() const;
    // Add source histogram data into local array
    void add(IntegerHistogram1D &other, int factor = 1);
    // Return current data
    Data1D data() const;
    // Return accumulated (averaged) data
    const Data1D &accumulatedData() const;

    /*
     * Operators
     */
    public:
    void operator=(const IntegerHistogram1D &source);
    IntegerHistogram1D operator+(const IntegerHistogram1D &other) const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
