// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "math/plottable.h"
#include "templates/array3d.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram3D;

// One-Dimensional Data
class Data3D : public PlottableData, public ListItem<Data3D>, public ObjectStore<Data3D>
{
    public:
    Data3D();
    virtual ~Data3D();
    Data3D(const Data3D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
    // X axis array
    std::vector<double> x_;
    // Y axis array
    std::vector<double> y_;
    // Z axis array
    std::vector<double> z_;
    // Values at each xyz
    Array3D<double> values_;
    // Whether data has associated errors
    bool hasError_;
    // Errors of values, if present
    Array3D<double> errors_;
    // Data version
    VersionCounter version_;

    public:
    // Initialise arrays to specified size
    void initialise(int xSize, int ySize, int zSize, bool withError = false);
    // Initialise to be consistent in size and axes with supplied object
    void initialise(const Data3D &source);
    // Copy arrays from supplied object
    void copyArrays(const Data3D &source);
    // Zero values array
    void zero();
    // Return data version
    int version() const;
    // Return x axis value specified
    double &xAxis(int index);
    const double &xAxis(int index) const;
    // Return x axis Array
    std::vector<double> &xAxis();
    const std::vector<double> &xAxis() const;
    // Return y axis value specified
    double &yAxis(int index);
    const double &yAxis(int index) const;
    // Return y axis Array
    std::vector<double> &yAxis();
    const std::vector<double> &yAxis() const;
    // Return z axis value specified
    double &zAxis(int index);
    const double &zAxis(int index) const;
    // Return z axis Array
    std::vector<double> &zAxis();
    const std::vector<double> &zAxis() const;
    // Return value specified
    double &value(int xIndex, int yIndex, int zIndex);
    const double &value(int xIndex, int yIndex, int zIndex) const;
    // Return three-dimensional values Array
    Array3D<double> &values3D();
    const Array3D<double> &values3D() const;
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
    double &error(int xIndex, int yIndex, int zIndex);
    const double &error(int xIndex, int yIndex, int zIndex) const;
    // Return three-dimensional errors Array
    Array3D<double> &errors3D();
    const Array3D<double> &errors3D() const;

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
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;

    /*
     * Parallel Comms
     */
    public:
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
