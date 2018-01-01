/*
	*** DataSpaceRange
	*** src/gui/uchroma/classes/dataspacerange.h
	Copyright T. Youngs 2012-2018.

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_UCHROMADATASPACERANGE_H
#define DUQ_UCHROMADATASPACERANGE_H

#include "gui/uchroma/classes/indexdata.h"
#include "gui/uchroma/classes/equationvariable.h"
#include "gui/uchroma/classes/namedvalue.h"
#include "templates/array.h"
#include "templates/array2d.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class Collection;
class DataSpace;
class Expression;
class Variable;
class ReferenceVariable;

/*
 * DataSpaceRange
 */
class DataSpaceRange : public ListItem<DataSpaceRange>
{
	public:
	// Constructor / Destructor
	DataSpaceRange(DataSpace& parent);
	~DataSpaceRange();

	private:
	// Parent
	DataSpace& parent_;


	/* 
	 * Target Data
	 */
	private:
	// Indices of first and last DisplayDataSet in this target
	int displayDataSetStart_, displayDataSetEnd_;
	// Number of sequential DisplayDataSet in this target
	int nDataSets_;
	// First and last abscissa indices in this target
	int abscissaStart_, abscissaEnd_;
	// Number of sequential abscissa values in this target
	int nPoints_;

	public:
	// Set target information
	void set(Collection* collection, int abscissaFirst, int abscissaLast, int firstDataSet, int lastDataSet, bool referenceDataOnly);
	// Set target information from existing DataSpaceRange
	void set(Collection* collection, DataSpaceRange* source, bool referenceDataOnly);
	// Return index of first DisplayDataSet to be fit
	int displayDataSetStart();
	// Return index of last DisplayDataSet to be fit
	int displayDataSetEnd();
	// Return number of sequential DisplayDataSets in this target
	int nDataSets();
	// Return first abscissa index in this target
	int abscissaStart();
	// Return last abscissa index in this target
	int abscissaEnd();
	// Return number of sequential abscissa values in this target
	int nPoints();
	// Return starting x value of range
	double xStart();
	// Return final x value of range
	double xEnd();
	// Return starting z value of range
	double zStart();
	// Return final z value of range
	double zEnd();


	/*
	 * Values
	 */
	private:
	// X values for target data
	Array<double> x_;
	// Z values for target data
	Array<double> z_;
	// Reference Y values
	Array2D<double> yReference_;
	// Reference Y types
	Array2D<double> yTypes_;
	// Calculated Y values
	Array2D<double> yCalculated_;
	// Fitted variable values
	List<NamedValue> fittedValues_;

	public:
	// Return reference y value specified
	double referenceY(int xIndex, int zIndex);
	// Return minimum of reference y values
	double referenceYMin();
	// Return maximum of reference y values
	double referenceYMax();
	// Return calculated y value specified
	double calculatedY(int xIndex, int zIndex);
	// Copy values from stored source collection, using index data provided
	bool copyValues(IndexData xIndex, IndexData zIndex);
	// Calculate values from specified equation
	bool calculateValues(Expression& equation, Variable* xVariable, Variable* zVariable, const RefList<ReferenceVariable,bool>& usedReferences);
	// Return sos error between stored and referenced values
	double sosError();
	// Add values to datasets in specified Collection
	void addCalculatedValues(Collection* target);
	// Add / set fitted variable value
	void setFittedValue(const char* name, double value);
	// Return whether named fitted value exists
	NamedValue* hasFittedValue(const char* name);
	// Return value of named fitted value
	double fittedValue(const char* name);
	// Return first in list of fitted values
	NamedValue* fittedValues();
};

#endif
