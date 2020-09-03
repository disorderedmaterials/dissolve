/*
    *** 3-Dimensional Data
    *** src/math/data3d.h
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
#include "templates/array3d.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram3D;

// One-Dimensional Data
class Data3D : public PlottableData, public ListItem<Data3D>, public ObjectStore<Data3D>, public GenericItemBase
{
    public:
    Data3D();
    ~Data3D();
    Data3D(const Data3D &source);
    // Clear data
    void clear();

    /*
     * Data
     */
    private:
    // X axis array
    Array<double> x_;
    // Y axis array
    Array<double> y_;
    // Z axis array
    Array<double> z_;
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
    // Return x axis value specified (const)
    double constXAxis(int index) const;
    // Return x axis Array
    Array<double> &xAxis();
    // Return x axis Array (const)
    const Array<double> &constXAxis() const;
    // Return y axis value specified
    double &yAxis(int index);
    // Return y axis value specified (const)
    double constYAxis(int index) const;
    // Return y axis Array
    Array<double> &yAxis();
    // Return y axis Array (const)
    const Array<double> &constYAxis() const;
    // Return z axis value specified
    double &zAxis(int index);
    // Return z axis value specified (const)
    double constZAxis(int index) const;
    // Return z axis Array
    Array<double> &zAxis();
    // Return z axis Array (const)
    const Array<double> &constZAxis() const;
    // Return value specified
    double &value(int xIndex, int yIndex, int zIndex);
    // Return value value specified (const)
    double constValue(int xIndex, int yIndex, int zIndex) const;
    // Return value Array
    Array3D<double> &values();
    // Return values Array
    const Array3D<double> &constValues3D() const;
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
    // Return error value specified (const)
    double constError(int xIndex, int yIndex, int zIndex) const;
    // Return error Array
    Array3D<double> &errors();
    // Return errors Array
    const Array3D<double> &constErrors3D() const;

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
