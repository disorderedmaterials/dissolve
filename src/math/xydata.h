/*
	*** XY Data
	*** src/math/xydata.h
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

#ifndef DISSOLVE_XYDATA_H
#define DISSOLVE_XYDATA_H

#include "base/charstring.h"
#include "math/broadeningfunction.h"
#include "math/windowfunction.h"
#include "templates/array.h"
#include "templates/objectstore.h"

#define OPTOLERANCE 1.0e-6

// Forward Declarations
class ProcessPool;

// XYData
class XYData : public ListItem<XYData>, public ObjectStore<XYData>, public GenericItemBase
{
	public:
	// Constructor
	XYData();
	// Destructor
	~XYData();
	// Copy Constructor
	XYData(const XYData& source);
	// Clear data
	void clear();
	

	/*
	 * Data
	 */
	private:
	// Array of x values
	Array<double> x_;
	// Array of y values
	Array<double> y_;
	// Z value
	double z_;
	// Name
	CharString name_;

	public:
	// Reset arrays to zero
	void reset();
	// Initialise arrays to specified size
	void initialise(int size);
	// Copy existing X and Y data
	void copyData(const XYData& source);
	// Copy existing X data and generate empty Y
	void templateFrom(const XYData& source);
	// Return number of defined datapoints
	int nPoints() const;
	// Return x value specified
	double& x(int index);
	// Return x value specified (const)
	double constX(int index) const;
	// Return x Array
	Array<double>& arrayX();
	// Return x Array (const)
	const Array<double>& constArrayX() const;
	// Return y value specified
	double& y(int index);
	// Return y value specified (const)
	double constY(int index) const;
	// Return y Array
	Array<double>& arrayY();
	// Return y Array (const)
	const Array<double>& constArrayY() const;
	// Add to z value
	void addZ(double delta);
	// Return z value
	double& z();
	// Return z value (const)
	double constZ() const;
	// Add new data point
	void addPoint(double x, double y);
	// Set name of data
	void setName(const char* name);
	// Return name of data
	const char* name() const;


	/*
	 * Operators
	 */
	public:
	// Assignment Operator
	void operator=(const XYData& source);
	// Operator +
	XYData operator+(const XYData& source) const;
	// Operator +=
	void operator+=(const XYData& source);
	// Operator +=
	void operator+=(const double dy);
	// Operator -
	XYData operator-(const XYData& source) const;
	// Operator -=
	void operator-=(const XYData& source);
	// Operator -=
	void operator-=(const double dy);
	// Operator *=
	void operator*=(const double factor);
	// Operator /=
	void operator/=(const double factor);


	/*
	 * Limits
	 */
	public:
	// Return minimum (first) x value in data
	double xMin() const;
	// Return maximum (last) x value in data
	double xMax() const;
	// Return minumum y value in data
	double yMin() const;
	// Return maxumum y value in data
	double yMax() const;


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
