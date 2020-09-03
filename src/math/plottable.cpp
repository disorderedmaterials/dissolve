/*
    *** Plottable Data
    *** src/math/plottable.cpp
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
const Array<double> &PlottableData::constYAxis() const
{
    Messenger::error("Tried to retrieve y axis array from a PlottableData that doesn't have a y axis.\n");
    static Array<double> dummy;
    return dummy;
}

// Return z axis Array (const)
const Array<double> &PlottableData::constZAxis() const
{
    Messenger::error("Tried to retrieve z axis array from a PlottableData that doesn't have a z axis.\n");
    static Array<double> dummy;
    return dummy;
}

/*
 * Values / Errors
 */

// Return values Array
const Array<double> &PlottableData::constValues() const
{
    Messenger::error(
        "Tried to retrieve a one-dimensional value array from a PlottableData that doesn't know how to return one.\n");
    static Array<double> dummy;
    return dummy;
}

// Return values Array
const Array2D<double> &PlottableData::constValues2D() const
{
    Messenger::error(
        "Tried to retrieve a two-dimensional value array from a PlottableData that doesn't know how to return one.\n");
    static Array2D<double> dummy;
    return dummy;
}

// Return three-dimensional values Array
const Array3D<double> &PlottableData::constValues3D() const
{
    Messenger::error("Tried to retrieve a three-dimensional value array from a PlottableData that doesn't know how to "
                     "return one.\n");
    static Array3D<double> dummy;
    return dummy;
}

// Return whether the values have associated errors
bool PlottableData::valuesHaveErrors() const { return false; }

// Return errors Array
const Array<double> &PlottableData::constErrors() const
{
    Messenger::error(
        "Tried to retrieve a one-dimensional error array from a PlottableData that doesn't know how to return one.\n");
    static Array<double> dummy;
    return dummy;
}

// Return errors Array
const Array2D<double> &PlottableData::constErrors2D() const
{
    Messenger::error(
        "Tried to retrieve a two-dimensional error array from a PlottableData that doesn't know how to return one.\n");
    static Array2D<double> dummy;
    return dummy;
}

// Return three-dimensional errors Array
const Array3D<double> &PlottableData::constErrors3D() const
{
    Messenger::error("Tried to retrieve a three-dimensional error array from a PlottableData that doesn't know how to "
                     "return one.\n");
    static Array3D<double> dummy;
    return dummy;
}
