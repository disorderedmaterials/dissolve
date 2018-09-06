/*
	*** Plottable Data
	*** src/math/plottable.h
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

#ifndef DISSOLVE_PLOTTABLE_H
#define DISSOLVE_PLOTTABLE_H

#include "math/sampleddouble.h"
#include "base/charstring.h"
#include "templates/array.h"

// Forward Declarations
template <class T> class Array3D;

// Plottable
class Plottable
{
	/*
	 * Basic Information
	 */
	public:
	// Plottable Types
	enum PlottableType {
		OneAxisPlottable, 		/* Contains series of data points against one axis */
		TwoDimensionalPlottable,	/* Contains data points plotted against two axes */
		ThreeDimensionalPlottable	/* Contains data points plotted againas three axes */
	};

	protected:
	// Name of plottable
	CharString name_;

	public:
	// Return type of plottable
	virtual PlottableType plottableType() const = 0;
	// Set name of plottable
	void setName(const char* name);
	// Return name of plottable
	const char* name() const;


	/*
	 * Axis Information
	 */
	public:
	// Return number of points along x axis
	virtual int nXAxisPoints() const = 0;
	// Return x axis Array
	virtual const Array<double>& xAxis() const = 0;
	// Return number of points along y axis
	virtual int nYAxisPoints() const;
	// Return y axis Array
	virtual const Array<double>& yAxis() const;
	// Return number of points along z axis
	virtual int nZAxisPoints() const;
	// Return z axis Array
	virtual const Array<double>& zAxis() const;


	/*
	 * Values / Errors
	 */
	public:
	// Return single value with index provided
	virtual double value(int xIndex) const;
	// Return values Array
	virtual const Array<double>& values() const;
	// Return single value with pair of indices provided
	virtual double value(int xIndex, int yIndex) const;
	// Return values Array
	virtual const Array2D<double>& values2D() const;
	// Return single value with triplet of indices provided
	virtual double value(int xIndex, int yIndex, int zIndex) const;
	// Return three-dimensional values Array
	virtual const Array3D<double>& values3D() const;
	// Return number of data points present in the whole dataset
	virtual int nDataPoints() const = 0;
	// Return minimum value over all data points
	virtual double minValue() const = 0;
	// Return maximum value over all data points
	virtual double maxValue() const = 0;
	// Return whether the values have associated errors
	virtual bool valuesHaveErrors() const;
	// Return single error with index provided
	virtual double error(int xIndex) const;
	// Return errors Array
	virtual const Array<double>& errors() const;
	// Return single error with pair of indices provided
	virtual double error(int xIndex, int yIndex) const;
	// Return errors Array
	virtual const Array2D<double>& errors2D() const;
	// Return single error with triplet of indices provided
	virtual double error(int xIndex, int yIndex, int zIndex) const;
	// Return three-dimensional errors Array
	virtual const Array3D<double>& errors3D() const;
};

#endif
