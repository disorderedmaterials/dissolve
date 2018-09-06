/*
	*** Plottable Data
	*** src/math/plottable.cpp
	Copyright T. Youngs 2012-2018

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

/*
 * Basic Information
 */

// Set name of plottable
void Plottable::setName(const char* name)
{
	name_ = name;
}

// Return name of plottable
const char* Plottable::name() const
{
	return name_.get();
}

/*
 * Axis Information
 */

// Return number of points along y axis
int Plottable::nYAxisPoints() const
{
	Messenger::error("Tried to retrieve number of y axis points from a Plottable that doesn't have a y axis.\n");
	return 0;
}

// Return y axis Array
const Array<double>& Plottable::yAxis() const
{
	Messenger::error("Tried to retrieve y axis array from a Plottable that doesn't have a y axis.\n");
	static Array<double> dummy;
	return dummy;
}

// Return number of points along z axis
int Plottable::nZAxisPoints() const
{
	Messenger::error("Tried to retrieve number of z axis points from a Plottable that doesn't have a z axis.\n");
	return 0;
}

// Return z axis Array
const Array<double>& Plottable::zAxis() const
{
	Messenger::error("Tried to retrieve z axis array from a Plottable that doesn't have a z axis.\n");
	static Array<double> dummy;
	return dummy;
}

/*
 * Values / Errors
 */

// Return single value with index provided
double Plottable::value(int xIndex) const
{
	Messenger::error("Tried to retrieve a value from a Plottable that doesn't understand a one-dimensional request.\n");
	return 0.0;
}

// Return values Array
const Array<double>& Plottable::values() const
{
	Messenger::error("Tried to retrieve a one-dimensional value array from a Plottable that doesn't know how to return one.\n");
	static Array<double> dummy;
	return dummy;
}

// Return single value with pair of indices provided
double Plottable::value(int xIndex, int yIndex) const
{
	Messenger::error("Tried to retrieve a value from a Plottable that doesn't understand a two-dimensional request.\n");
	return 0.0;
}

// Return values Array
const Array2D<double>& Plottable::values2D() const
{
	Messenger::error("Tried to retrieve a two-dimensional value array from a Plottable that doesn't know how to return one.\n");
	static Array2D<double> dummy;
	return dummy;
}

// Return single value with triplet of indices provided
double Plottable::value(int xIndex, int yIndex, int zIndex) const
{
	Messenger::error("Tried to retrieve a value from a Plottable that doesn't understand a three-dimensional request.\n");
	return 0.0;
}

// Return three-dimensional values Array
const Array3D<double>& Plottable::values3D() const
{
	Messenger::error("Tried to retrieve a three-dimensional value array from a Plottable that doesn't know how to return one.\n");
	static Array3D<double> dummy;
	return dummy;
}

// Return whether the values have associated errors
bool Plottable::valuesHaveErrors() const
{
	return false;
}

// Return single error with index provided
double Plottable::error(int xIndex) const
{
	Messenger::error("Tried to retrieve an error from a Plottable that doesn't understand a one-dimensional request.\n");
	return 0.0;
}

// Return errors Array
const Array<double>& Plottable::errors() const
{
	Messenger::error("Tried to retrieve a one-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array<double> dummy;
	return dummy;
}

// Return single error with pair of indices provided
double Plottable::error(int xIndex, int yIndex) const
{
	Messenger::error("Tried to retrieve an error from a Plottable that doesn't understand a two-dimensional request.\n");
	return 0.0;
}

// Return errors Array
const Array2D<double>& Plottable::errors2D() const
{
	Messenger::error("Tried to retrieve a two-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array2D<double> dummy;
	return dummy;
}

// Return single error with triplet of indices provided
double Plottable::error(int xIndex, int yIndex, int zIndex) const
{
	Messenger::error("Tried to retrieve an error from a Plottable that doesn't understand a three-dimensional request.\n");
	return 0.0;
}

// Return three-dimensional errors Array
const Array3D<double>& Plottable::errors3D() const
{
	Messenger::error("Tried to retrieve a three-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array3D<double> dummy;
	return dummy;
}
