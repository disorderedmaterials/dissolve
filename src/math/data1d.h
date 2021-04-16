// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/version.h"
#include "math/data1dbase.h"
#include <string>

// One-Dimensional Data
class Data1D : public Data1DBase
{
    public:
    Data1D();
    virtual ~Data1D() = default;
    Data1D(const Data1D &source);
    Data1D(const Data1DBase &source);
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
    std::vector<double> values_;
    // Whether data has associated errors
    bool hasError_;
    // Errors of values, if present
    std::vector<double> errors_;
    // Data version
    VersionCounter version_;

    public:
    // Set tag
    void setTag(std::string_view tag);
    // Return tag
    std::string_view tag() const;
    // Initialise arrays to specified size
    void initialise(int size, bool withError = false);
    // Initialise to be consistent in size and x axis with supplied object
    void initialise(const Data1D &source);
    // Copy arrays from supplied object
    void copyArrays(const Data1D &source);
    // Zero values array
    void zero();
    // Return data version
    int version() const;
    // Add new data point
    void addPoint(double x, double value);
    // Add new data point with error
    void addPoint(double x, double value, double error);
    // Remove first point
    void removeFirstPoint();
    // Remove last point
    void removeLastPoint();
    // Return x axis value specified
    double &xAxis(int index);
    const double &xAxis(int index) const;
    // Return x axis vector
    std::vector<double> &xAxis();
    const std::vector<double> &xAxis() const;
    // Return value specified
    double &value(int index);
    const double &value(int index) const;
    // Return value Array
    std::vector<double> &values();
    const std::vector<double> &values() const;
    // Return number of values present in whole dataset
    int nValues() const;
    // Return minimum value over all data points
    double minValue() const;
    // Return maximum value over all data points
    double maxValue() const;
    // Add / initialise errors array
    void addErrors();
    // Return whether the values have associated errors
    bool valuesHaveErrors() const;
    // Return error value specified
    double &error(int index);
    const double &error(int index) const;
    // Return error Array
    std::vector<double> &errors();
    // Return errors Array
    const std::vector<double> &errors() const;

    /*
     * Operators
     */
    public:
    void operator=(const Data1D &source);
    void operator+=(const Data1D &source);
    void operator+=(const double delta);
    void operator-=(const Data1D &source);
    void operator-=(const double delta);
    void operator*=(const double factor);
    void operator*=(const std::vector<double> &factors);
    void operator/=(const double factor);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
};
