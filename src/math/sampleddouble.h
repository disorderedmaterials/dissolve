// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "genericitems/base.h"
#include <ctime>

// Forward Declarations
class ProcessPool;

// Double value with sampling
class SampledDouble : public GenericItemBase
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
    void operator=(double x);
    void operator=(const SampledDouble &source);
    void operator+=(double x);
    void operator+=(int i);
    void operator+=(const SampledDouble &source);
    void operator*=(double factor);
    void operator/=(double factor);

    /*
     * GenericItemBase Implementations
     */
    public:
    // Return class name
    static std::string_view itemClassName();
    // Read data through specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);

    /*
     * Parallel Comms
     */
    public:
    // Sum data over all processes within the pool
    bool allSum(ProcessPool &procPool);
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check equality of all data
    bool equality(ProcessPool &procPool);
};
