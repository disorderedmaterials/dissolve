// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

// Forward Declarations
class CoreData;
class LineParser;
class ProcessPool;

// Double value with sampling
class SampledDouble
{
    public:
    SampledDouble();
    SampledDouble(const double x);

    /*
     * Data
     */
    private:
    // Sample size contributing to averages etc.
    int count_;
    // Mean of sampled data (i.e. current value)
    double mean_;
    // Aggregate of squared distance from mean
    double m2_;

    public:
    // Clear data
    void clear();
    // Return current (mean) value
    double value() const;
    // Return number of samples contributing to averages etc.
    int count() const;
    // Return variance of sampled data
    double variance() const;
    // Return standard deviation of sampled data
    double stDev() const;

    /*
     * Operators
     */
    public:
    operator double &();
    operator const double &() const;
    SampledDouble &operator=(double x);
    SampledDouble &operator=(const SampledDouble &source);
    SampledDouble operator+(double x) const;
    SampledDouble operator*(double x) const;
    SampledDouble operator/(double x) const;
    void operator+=(double x);
    void operator+=(int i);
    void operator+=(const SampledDouble &source);
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

    /*
     * Parallel Comms
     */
    public:
    // Sum data over all processes within the pool
    bool allSum(ProcessPool &procPool);
};
