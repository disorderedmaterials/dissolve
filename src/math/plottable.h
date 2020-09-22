// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "math/sampleddouble.h"
#include "templates/array.h"

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
    virtual const Array<double> &constXAxis() const = 0;
    // Return y axis Array
    virtual const Array<double> &constYAxis() const;
    // Return z axis Array
    virtual const Array<double> &constZAxis() const;

    /*
     * Values / Errors
     */
    public:
    // Return data version
    virtual int version() const = 0;
    // Return values Array
    virtual const Array<double> &constValues() const;
    // Return values Array
    virtual const Array2D<double> &constValues2D() const;
    // Return three-dimensional values Array
    virtual const Array3D<double> &constValues3D() const;
    // Return number of values present in the whole dataset
    virtual int nValues() const = 0;
    // Return minimum value over all data points
    virtual double minValue() const = 0;
    // Return maximum value over all data points
    virtual double maxValue() const = 0;
    // Return whether the values have associated errors
    virtual bool valuesHaveErrors() const;
    // Return errors Array
    virtual const Array<double> &constErrors() const;
    // Return errors Array
    virtual const Array2D<double> &constErrors2D() const;
    // Return three-dimensional errors Array
    virtual const Array3D<double> &constErrors3D() const;
};
