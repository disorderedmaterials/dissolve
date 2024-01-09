// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <vector>

// Forward Declarations
class CoreData;
class LineParser;

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
    int count_{0};
    // Mean of sampled data (i.e. current values)
    std::vector<double> mean_;
    // Aggregate of squared distance from mean values
    std::vector<double> m2_;
    // Standard deviations of values
    std::vector<double> stDev_;

    public:
    // Initialise arrays
    void initialise(int nValues);
    // Initialise from pre-existing arrays
    void initialise(int count, const std::vector<double> &mean, const std::vector<double> &stDev,
                    const std::vector<double> &m2);
    // Clear all arrays
    void clear();
    // Reset values and statistics
    void reset();
    // Return number of samples contributing to averages etc.
    int count() const;
    // Return current (mean) values
    const std::vector<double> &values() const;
    // Return standard deviations of values
    const std::vector<double> &stDev() const;
    // Return squared deviations
    const std::vector<double> &m2() const;

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
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
