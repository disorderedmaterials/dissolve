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

// Constructor
Plottable::Plottable::Plottable(Plottable::PlottableType type)
{
	type_ = type;
}

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

// Return y axis Array (const)
const Array<double>& Plottable::constYAxis() const
{
	Messenger::error("Tried to retrieve y axis array from a Plottable that doesn't have a y axis.\n");
	static Array<double> dummy;
	return dummy;
}

// Return z axis Array (const)
const Array<double>& Plottable::constZAxis() const
{
	Messenger::error("Tried to retrieve z axis array from a Plottable that doesn't have a z axis.\n");
	static Array<double> dummy;
	return dummy;
}

/*
 * Values / Errors
 */

// Return values Array
const Array<double>& Plottable::constValues() const
{
	Messenger::error("Tried to retrieve a one-dimensional value array from a Plottable that doesn't know how to return one.\n");
	static Array<double> dummy;
	return dummy;
}

// Return values Array
const Array2D<double>& Plottable::constValues2D() const
{
	Messenger::error("Tried to retrieve a two-dimensional value array from a Plottable that doesn't know how to return one.\n");
	static Array2D<double> dummy;
	return dummy;
}

// Return three-dimensional values Array
const Array3D<double>& Plottable::constValues3D() const
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

// Return errors Array
const Array<double>& Plottable::constErrors() const
{
	Messenger::error("Tried to retrieve a one-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array<double> dummy;
	return dummy;
}

// Return errors Array
const Array2D<double>& Plottable::constErrors2D() const
{
	Messenger::error("Tried to retrieve a two-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array2D<double> dummy;
	return dummy;
}

// Return three-dimensional errors Array
const Array3D<double>& Plottable::constErrors3D() const
{
	Messenger::error("Tried to retrieve a three-dimensional error array from a Plottable that doesn't know how to return one.\n");
	static Array3D<double> dummy;
	return dummy;
}
