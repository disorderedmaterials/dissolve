// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <vector>

// Forward Declarations
class CoreData;
class LineParser;
class ProcessPool;

// Vector of double values with sampling
class SampledVector
{
    public:
    SampledVector();
    ~SampledVector() = default;

    /*
     * Data
     */
    private:
    // Sample size contributing to averages etc.
    int count_;
    // Mean of sampled data (i.e. current values)
    std::vector<double> mean_;
    // Aggregate of squared distance from mean values
    std::vector<double> m2_;
    // Standard deviations of values
    std::vector<double> stDev_;

    public:
    // Initialise arrays
    void initialise(int nValues);
    // Reset values and statistics
    void reset();
    // Return number of samples contributing to averages etc.
    int count() const;
    // Return current (mean) values
    const std::vector<double> &values() const;
    // Return standard deviations of values
    const std::vector<double> &stDev() const;

    /*
     * Operators
     */
    public:
    SampledVector &operator+=(const std::vector<double> &source);
    SampledVector &operator=(const SampledVector &source);
    SampledVector &operator+=(const SampledVector &source);
    void operator*=(double factor);
    void operator/=(double factor);

    /*
     * GenericItemBase Implementations
     */
    public:
    // Read data through specified LineParser
    bool read(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check equality of all data
    bool equality(ProcessPool &procPool);
};
