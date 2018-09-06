/*
	*** 1-Dimensional Data
	*** src/math/data1d.h
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

#ifndef DISSOLVE_DATA1D_H
#define DISSOLVE_DATA1D_H

#include "math/plottable.h"
#include "templates/array.h"
#include "templates/objectstore.h"

// Forward Declarations
class Histogram1D;

// One-Dimensional Data
class Data1D : public Plottable, public ListItem<Data1D>, public ObjectStore<Data1D>, public GenericItemBase
{
	public:
	// Constructor
	Data1D();
	// Destructor
	~Data1D();
	// Copy Constructor
	Data1D(const Data1D& source);
	// Clear data
	void clear();
	

	/*
	 * Data
	 */
	private:
	// X array
	Array<double> x_;
	// Accumulated values at each x
	Array<double> y_;
	// Whether data has associated errors
	bool hasError_;
	// Errors of accumulated values, if present
	Array<double> error_;

	public:
	// Initialise arrays to specified size
	void initialise(int size, bool withError = false);
	// Initialise to be consistent in size and x axis with supplied object
	void initialise(const Data1D& source);
	// Copy arrays from supplied object
	void copyArrays(const Data1D& source);
	// Zero values array
	void zero();
	// Accumulate specified histogram data
	void accumulate(const Histogram1D& source);
	// Add new data point
	void addPoint(double x, double y);
	// Add new data point with error
	void addPoint(double x, double y, double yError);
	// Return x value specified
	double& x(int index);
	// Return x value specified (const)
	double constX(int index) const;
	// Return x Array
	Array<double>& x();
	// Return x Array (const)
	const Array<double>& constX() const;
	// Return y value specified
	double& y(int index);
	// Return y value specified (const)
	double constY(int index) const;
	// Return y Array
	Array<double>& y();
	// Return y Array (const)
	const Array<double>& constY() const;
	// Add / initialise errors array
	void addErrors();
	// Return y error value specified
	double& yError(int index);
	// Return y error value specified (const)
	double constYError(int index) const;
	// Return y error Array
	Array<double>& yError();
	// Return y error Array (const)
	const Array<double>& constYError() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const Data1D& source);
	// Operator +=
	void operator+=(const Data1D& source);
	// Operator +=
	void operator+=(const double dy);
	// Operator -=
	void operator-=(const Data1D& source);
	// Operator -=
	void operator-=(const double dy);
	// Operator *=
	void operator*=(const double factor);
	// Operator /=
	void operator/=(const double factor);


	/*
	 * File I/O
	 */
	public:
	// Load data from specified LineParser, using columns specified
	bool load(LineParser& parser, int xcol = 0, int ycol = 1);
	// Load data from specified file, using columns specified
	bool load(const char* filename, int xcol = 0, int ycol = 1);
	// Load data from specified file through ProcessPool, using columns specified
	bool load(ProcessPool& pool, const char* filename, int xcol = 0, int ycol = 1);
	// Save data to specified file
	bool save(const char* filename) const;


	/*
	 * Plottable Implementation
	 */
	public:
	// Return type of plottable
	PlottableType plottableType() const;
	// Return number of points along x axis
	int nXAxisPoints() const;
	// Return x axis value specified
	double xAxis(int index) const;
	// Return minimum (first) x axis point
	double xAxisMin() const;
	// Return maximum (last) x axis point
	double xAxisMax() const;
	// Return x axis Array
	const Array<double>& xAxis() const;
	// Return single value with index provided
	double value(int xIndex) const;
	// Return values Array
	const Array<double>& values() const;
	// Return number of datapoints present in whole dataset
	int nDataPoints() const;
	// Return minimum value over all data points
	double minValue() const;
	// Return maximum value over all data points
	double maxValue() const;
	// Return whether the values have associated errors
	bool valuesHaveErrors() const;
	// Return single error with index provided
	double error(int xIndex) const;
	// Return errors Array
	const Array<double>& errors() const;


	/*
	 * GenericItemBase Implementations
	 */
	public:
	// Return class name
	static const char* itemClassName();
	// Write data through specified LineParser
	bool write(LineParser& parser);
	// Read data through specified LineParser
	bool read(LineParser& parser);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data
	bool broadcast(ProcessPool& procPool, int rootRank = 0);
	// Check item equality
	bool equality(ProcessPool& procPool);
};

#endif
