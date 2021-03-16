// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "math/plottable.h"
#include "templates/objectstore.h"

// One-Dimensional Data
class Data1D : public PlottableData, public ListItem<Data1D>, public ObjectStore<Data1D>
{
    public:
    Data1D();
    virtual ~Data1D();
    Data1D(const Data1D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
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
    // Return x axis Array
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
