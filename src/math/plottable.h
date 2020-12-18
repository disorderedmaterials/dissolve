// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "math/sampleddouble.h"
#include "templates/array.h"
#include <vector>

// Forward Declarations
template <class T> class Array3D;

// Plottable
class PlottableData
{
    public:
    // Plottable Types
    enum PlottableDataType
    {
        OneAxisPlottable,  /* Contains data points plotted against one axis (x) */
        TwoAxisPlottable,  /* Contains data points plotted against two axes (x and y) */
        ThreeAxisPlottable /* Contains data points plotted againas three axes (x, y, and z) */
    };
    PlottableData(PlottableDataType type);

    /*
     * Basic Information
     */
    private:
    // Type of plottable
    PlottableDataType type_;

    protected:
    // Name of plottable
    std::string name_;

    public:
    // Set name of plottable
    void setName(std::string_view name);
    // Return name of plottable
    std::string_view name() const;

    /*
     * Axis Information
     */
    public:
    // Return x axis Array
    virtual const std::vector<double> &xAxis() const = 0;
    // Return y axis Array
    virtual const std::vector<double> &yAxis() const;
    // Return z axis Array
    virtual const std::vector<double> &zAxis() const;

    /*
     * Values / Errors
     */
    public:
    // Return data version
    virtual int version() const = 0;
    // Return values Array
    virtual const std::vector<double> &values() const;
    // Return values Array
    virtual const Array2D<double> &values2D() const;
    // Return three-dimensional values Array
    virtual const Array3D<double> &values3D() const;
    // Return number of values present in the whole dataset
    virtual int nValues() const = 0;
    // Return minimum value over all data points
    virtual double minValue() const = 0;
    // Return maximum value over all data points
    virtual double maxValue() const = 0;
    // Return whether the values have associated errors
    virtual bool valuesHaveErrors() const;
    // Return errors Array
    virtual const std::vector<double> &errors() const;
    // Return errors Array
    virtual const Array2D<double> &errors2D() const;
    // Return three-dimensional errors Array
    virtual const Array3D<double> &errors3D() const;
};
