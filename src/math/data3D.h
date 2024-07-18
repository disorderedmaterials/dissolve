// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "math/data3DBase.h"
#include "templates/array3D.h"

// Forward Declarations
class Histogram3D;

// One-Dimensional Data
class Data3D : public Data3DBase
{
    public:
    Data3D();
    virtual ~Data3D() = default;
    Data3D(const Data3D &source);
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
    // Z axis array
    std::vector<double> z_;
    // Values at each xyz
    Array3D<double> values_;
    // Whether data has associated errors
    bool hasError_{false};
    // Errors of values, if present
    Array3D<double> errors_;
    // Data version
    VersionCounter version_;

    public:
    // Set tag
    void setTag(std::string_view tag);
    // Return tag
    std::string_view tag() const;
    // Initialise arrays to specified size
    void initialise(int xSize, int ySize, int zSize, bool withError = false);
    // Initialise arrays to specified uniform size
    void initialise(int size, bool withError = false);
    // Initialise bins and axes simultaneously
    void initialise(int nx, double xMin, double xDelta, int ny, double yMin, double yDelta, int nz, double zMin, double zDelta,
                    bool withError = false);
    // Initialise to be consistent in size and axes with supplied object
    void initialise(const Data3D &source);
    // Copy arrays from supplied object
    void copyArrays(const Data3D &source);
    // Zero values array
    void zero();
    // Return data version
    int version() const override;
    // Add new data point
    void addPoint(double x, double y, double z, double value);
    // Add data to point
    void addToPoint(int x, double xDelta, int y, double yDelta, int z, double zDelta, double value);
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
    // Return z axis value specified
    double &zAxis(int index);
    const double &zAxis(int index) const;
    // Return z axis Array
    std::vector<double> &zAxis();
    const std::vector<double> &zAxis() const override;
    // Return value specified
    double &value(int xIndex, int yIndex, int zIndex);
    const double &value(int xIndex, int yIndex, int zIndex) const;
    // Return three-dimensional values Array
    Array3D<double> &values();
    const Array3D<double> &values() const override;
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
    double &error(int xIndex, int yIndex, int zIndex);
    const double &error(int xIndex, int yIndex, int zIndex) const;
    // Return three-dimensional errors Array
    Array3D<double> &errors();
    const Array3D<double> &errors() const override;

    /*
     * Operators
     */
    public:
    void operator=(const Data3D &source);
    void operator+=(const Data3D &source);
    void operator+=(const double delta);
    void operator-=(const Data3D &source);
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
};
