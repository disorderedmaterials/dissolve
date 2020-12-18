// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "math/data1d.h"
#include "math/plottable.h"
#include "templates/array2d.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram2D;

// One-Dimensional Data
class Data2D : public PlottableData, public ListItem<Data2D>, public ObjectStore<Data2D>, public GenericItemBase
{
    public:
    Data2D();
    virtual ~Data2D();
    Data2D(const Data2D &source);
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
    // Values at each xy
    Array2D<double> values_;
    // Whether data has associated errors
    bool hasError_;
    // Errors of values, if present
    Array2D<double> errors_;
    // Data version
    VersionCounter version_;

    public:
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
    // Return value specified
    double &value(int xIndex, int yIndex);
    const double &value(int xIndex, int yIndex) const;
    // Return value Array
    Array2D<double> &values();
    // Return values Array
    const Array2D<double> &values2D() const;
    // Return value specified from linear array
    double value(int index);
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
    double &error(int xIndex, int yIndex);
    const double &error(int xIndex, int yIndex) const;
    // Return error Array
    Array2D<double> &errors();
    // Return errors Array
    const Array2D<double> &errors2D() const;

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
    // Broadcast data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check item equality
    bool equality(ProcessPool &procPool);
};
