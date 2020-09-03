/*
    *** 1-Dimensional Data
    *** src/math/data1d.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/version.h"
#include "math/plottable.h"
#include "templates/array.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram1D;

// One-Dimensional Data
class Data1D : public PlottableData, public ListItem<Data1D>, public ObjectStore<Data1D>, public GenericItemBase
{
    public:
    Data1D();
    ~Data1D();
    Data1D(const Data1D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
    // X array
    Array<double> x_;
    // Values at each x
    Array<double> values_;
    // Whether data has associated errors
    bool hasError_;
    // Errors of values, if present
    Array<double> errors_;
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
    // Return x axis value specified (const)
    double constXAxis(int index) const;
    // Return x axis Array
    Array<double> &xAxis();
    // Return x axis Array (const)
    const Array<double> &constXAxis() const;
    // Return value specified
    double &value(int index);
    // Return value value specified (const)
    double constValue(int index) const;
    // Return value Array
    Array<double> &values();
    // Return values Array
    const Array<double> &constValues() const;
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
    // Return error value specified (const)
    double constError(int index) const;
    // Return error Array
    Array<double> &errors();
    // Return errors Array
    const Array<double> &constErrors() const;

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
