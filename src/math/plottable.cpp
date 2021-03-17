// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "math/plottable.h"
#include "templates/array2d.h"
#include "templates/array3d.h"

PlottableData::PlottableData(PlottableData::PlottableDataType type) { type_ = type; }

/*
 * Basic Information
 */

// Set name of plottable
void PlottableData::setName(std::string_view name) { name_ = name; }

// Return name of plottable
std::string_view PlottableData::name() const { return name_; }

/*
 * Axis Information
 */

// Return y axis Array (const)
const std::vector<double> &PlottableData::yAxis() const
{
    Messenger::error("Tried to retrieve y axis array from a PlottableData that doesn't have a y axis.\n");
    static std::vector<double> dummy;
    return dummy;
}

// Return z axis Array (const)
const std::vector<double> &PlottableData::zAxis() const
{
    Messenger::error("Tried to retrieve z axis array from a PlottableData that doesn't have a z axis.\n");
    static std::vector<double> dummy;
    return dummy;
}

/*
 * Values / Errors
 */

// Return values Array
const std::vector<double> &PlottableData::values() const
{
    Messenger::error(
        "Tried to retrieve a one-dimensional value array from a PlottableData that doesn't know how to return one.\n");
    static std::vector<double> dummy;
    return dummy;
}

// Return two-dimensional values Array
const Array2D<double> &PlottableData::values2D() const
{
    Messenger::error(
        "Tried to retrieve a two-dimensional value array from a PlottableData that doesn't know how to return one.\n");
    static Array2D<double> dummy;
    return dummy;
}

// Return three-dimensional values Array
const Array3D<double> &PlottableData::values3D() const
{
    Messenger::error("Tried to retrieve a three-dimensional value array from a PlottableData that doesn't know how to "
                     "return one.\n");
    static Array3D<double> dummy;
    return dummy;
}

// Return whether the values have associated errors
bool PlottableData::valuesHaveErrors() const { return false; }

// Return errors Array
const std::vector<double> &PlottableData::errors() const
{
    Messenger::error(
        "Tried to retrieve a one-dimensional error array from a PlottableData that doesn't know how to return one.\n");
    static std::vector<double> dummy;
    return dummy;
}

// Return two-dimensional errors Array
const Array2D<double> &PlottableData::errors2D() const
{
    Messenger::error(
        "Tried to retrieve a two-dimensional error array from a PlottableData that doesn't know how to return one.\n");
    static Array2D<double> dummy;
    return dummy;
}

// Return three-dimensional errors Array
const Array3D<double> &PlottableData::errors3D() const
{
    Messenger::error("Tried to retrieve a three-dimensional error array from a PlottableData that doesn't know how to "
                     "return one.\n");
    static Array3D<double> dummy;
    return dummy;
}
