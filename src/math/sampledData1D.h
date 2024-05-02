// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "math/data1DBase.h"
#include "math/sampledVector.h"
#include <string>

// Forward Declarations
class Data1D;

// One-Dimensional Data with Statistics
class SampledData1D : public Data1DBase
{
    public:
    SampledData1D();
    ~SampledData1D() = default;
    SampledData1D(const SampledData1D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
    // Tag for data (optional)
    std::string tag_;
    // X array
    std::vector<double> x_;
    // Values at each x
    SampledVector values_;
    // Data version
    VersionCounter version_;

    public:
    // Set tag
    void setTag(std::string_view tag);
    // Return tag
    std::string_view tag() const;
    // Initialise arrays to specified size
    void initialise(int size);
    // Initialise arrays, copying abscissa values
    void initialise(const std::vector<double> &xAxis);

    // Zero values array
    void zero();
    // Return data version
    int version() const override;
    // Return x axis value specified
    const double &xAxis(int index) const;
    // Return x axis vector
    const std::vector<double> &xAxis() const override;
    // Return value specified
    const double &value(int index) const;
    // Return value vector
    const std::vector<double> &values() const override;
    // Return number of values present in whole dataset
    int nValues() const override;
    // Return minimum value over all data points
    double minValue() const override;
    // Return maximum value over all data points
    double maxValue() const override;
    // Return whether the values have associated errors
    bool valuesHaveErrors() const override;
    // Return error value specified
    const double &error(int index) const;
    // Return errors vector
    const std::vector<double> &errors() const override;

    /*
     * Operators
     */
    public:
    SampledData1D &operator=(const Data1D &source);
    void operator+=(const Data1D &source);
    void operator+=(const std::vector<double> &source);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Read value data only through specified LineParser
    bool deserialiseValues(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
    // Write value data only through specified LineParser
    bool serialiseValues(LineParser &parser) const;
};
