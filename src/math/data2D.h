// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "base/version.h"
#include "math/data2DBase.h"
#include "templates/array2D.h"

// Forward Declarations
class Histogram2D;

// One-Dimensional Data
class Data2D : public Data2DBase, public Serialisable<>
{
    public:
    Data2D();
    virtual ~Data2D() = default;
    Data2D(const Data2D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
    // Tag for data (optional)
    std::string tag_;
    // X axis array
    std::vector<double> x_;
    // Y axis array
    std::vector<double> y_;
    // Values at each xy
    Array2D<double> values_;
    // Whether data has associated errors
    bool hasError_{false};
    // Errors of values, if present
    Array2D<double> errors_;
    // Data version
    VersionCounter version_;

    public:
    // Set tag
    void setTag(std::string_view tag);
    // Return tag
    std::string_view tag() const;
    // Initialise arrays to specified size
    void initialise(int xSize, int ySize, bool withError = false);
    // Initialise to be consistent in size and axes with supplied object
    void initialise(const Data2D &source);
    // Initialise from supplied axis ranges
    void initialise(double xMin, double xMax, double xBin, double yMin, double yMax, double yBin, bool withError = false);
    // Copy arrays from supplied object
    void copyArrays(const Data2D &source);
    // Zero values array
    void zero();
    // Return data version
    int version() const override;
    // Return x axis value specified
    double &xAxis(int index);
    const double &xAxis(int index) const;
    // Return x axis vector
    std::vector<double> &xAxis();
    const std::vector<double> &xAxis() const override;
    // Return y axis value specified
    double &yAxis(int index);
    const double &yAxis(int index) const;
    // Return y axis vector
    std::vector<double> &yAxis();
    const std::vector<double> &yAxis() const override;
    // Return value specified
    double &value(int xIndex, int yIndex);
    const double &value(int xIndex, int yIndex) const;
    // Return two-dimensional values Array
    Array2D<double> &values();
    const Array2D<double> &values() const override;
    // Return value specified from linear array
    double value(int index);
    // Return number of values present in whole dataset
    int nValues() const override;
    // Return minimum value over all data points
    double minValue() const override;
    // Return maximum value over all data points
    double maxValue() const override;
    // Add / initialise errors array
    void addErrors();
    // Return whether the values have associated errors
    bool valuesHaveErrors() const override;
    // Return error value specified
    double &error(int xIndex, int yIndex);
    const double &error(int xIndex, int yIndex) const;
    // Return two-dimensional errors Array
    Array2D<double> &errors();
    const Array2D<double> &errors() const override;

    /*
     * Operators
     */
    public:
    void operator=(const Data2D &source);
    void operator+=(const double delta);
    void operator-=(const double delta);
    void operator*=(const double factor);
    void operator/=(const double factor);

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
