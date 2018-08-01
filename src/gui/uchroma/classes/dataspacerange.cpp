/*
	*** DataSpaceRange
	*** src/gui/uchroma/classes/dataspacerange.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/uchroma/classes/dataspace.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/classes/referencevariable.h"
#include "gui/uchroma/classes/viewpane.h"
#include "gui/uchroma/expression/expression.h"
#include "gui/uchroma/expression/variable.h"
#include "base/sysfunc.h"

// Constructor
DataSpaceRange::DataSpaceRange(DataSpace& parent) : ListItem<DataSpaceRange>(), parent_(parent)
{
	displayDataSetStart_ = -1;
	displayDataSetEnd_ = -1;
	nDataSets_ = 0;
	abscissaStart_ = -1;
	abscissaEnd_ = -1;
	nPoints_ = 0;
}

// Destructor
DataSpaceRange::~DataSpaceRange()
{
}

/* 
 * Target Data
 */

// Set target information
void DataSpaceRange::set(Collection* collection, int abscissaFirst, int abscissaLast, int firstDataSet, int lastDataSet, bool referenceDataOnly)
{
	// Check for valid collection
	if (!Collection::objectValid(collection, "collection in DataSpaceRange::set()")) return;

	displayDataSetStart_ = firstDataSet;
	displayDataSetEnd_ = lastDataSet;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	abscissaStart_ = abscissaFirst;
	abscissaEnd_ = abscissaLast;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;

	// Setup data arrays
	x_.clear();
	z_.clear();
	yReference_.clear();
	yTypes_.clear();
	yCalculated_.clear();

	const Array<double>& abscissa = collection->displayAbscissa();
	DisplayDataSet** dataSets = collection->displayData().array();

	// Store x values
	for (int n=0; n<nPoints_; ++n) x_.add(abscissa.value(n+abscissaStart_));

	// Store z values
	for (int n=0; n<nDataSets_; ++n) z_.add(dataSets[n+displayDataSetStart_]->z());

	// Copy y data
	yReference_.initialise(nPoints_, nDataSets_);
	if (!referenceDataOnly)
	{
		yTypes_.initialise(nPoints_, nDataSets_);
		yCalculated_.initialise(nPoints_, nDataSets_);
	}

	for (int n=0; n<nDataSets_; ++n)
	{
		const Array<double>& y = dataSets[n+displayDataSetStart_]->y();
		const Array<DisplayDataSet::DataPointType>& yType = dataSets[n+displayDataSetStart_]->yType();
		for (int i=0; i<nPoints_; ++i)
		{
			yReference_.at(i,n) = y.value(i+abscissaStart_);
			if (!referenceDataOnly) yTypes_.at(i,n) = yType.value(i+abscissaStart_);
		}
	}
}

// Set target information from existing DataSpaceRange
void DataSpaceRange::set(Collection* collection, DataSpaceRange* source, bool referenceDataOnly)
{
	set(collection, source->abscissaStart_, source->abscissaEnd_, source->displayDataSetStart_, source->displayDataSetEnd_, referenceDataOnly);
}

// Return index of first DisplayDataSet in this target
int DataSpaceRange::displayDataSetStart()
{
	return displayDataSetStart_;
}

// Return index of last DisplayDataSet in this target
int DataSpaceRange::displayDataSetEnd()
{
	return displayDataSetEnd_;
}

// Return number of sequential DisplayDataSets in this target
int DataSpaceRange::nDataSets()
{
	return nDataSets_;
}

// Return first abscissa index in this target
int DataSpaceRange::abscissaStart()
{
	return abscissaStart_;
}

// Return last abscissa index in this target
int DataSpaceRange::abscissaEnd()
{
	return abscissaEnd_;
}

// Return number of sequential abscissa values in this target
int DataSpaceRange::nPoints()
{
	return nPoints_;
}

// Return starting x value of range
double DataSpaceRange::xStart()
{
	// Check for valid collection
	if (x_.nItems() == 0)
	{
		Messenger::print("DataSpaceRange::xStart() - x_ array is empty.\n");
		return 0.0;
	}

	return x_.first();
}

// Return final x value of range
double DataSpaceRange::xEnd()
{
	// Check for valid collection
	if (x_.nItems() == 0)
	{
		Messenger::print("DataSpaceRange::xEnd() - x_ array is empty.\n");
		return 0.0;
	}

	return x_.last();
}

// Return starting z value of range
double DataSpaceRange::zStart()
{
	// Check for valid collection
	if (z_.nItems() == 0)
	{
		Messenger::print("DataSpaceRange::zStart() - z_ array is empty.\n");
		return 0.0;
	}

	return z_.first();
}

// Return final z value of range
double DataSpaceRange::zEnd()
{
	// Check for valid collection
	if (z_.nItems() == 0)
	{
		Messenger::print("DataSpaceRange::zEnd() - z_ array is empty.\n");
		return 0.0;
	}

	return z_.last();
}

/*
 * Values
 */

// Return reference y value specified
double DataSpaceRange::referenceY(int xIndex, int zIndex)
{
	return yReference_.at(xIndex, zIndex);
}

// Return minimum of reference y values
double DataSpaceRange::referenceYMin()
{
	if (yReference_.linearArraySize() == 0) return 0.0;
	
	double minVal = yReference_.linearArray()[0];
	for (int n=1; n<yReference_.linearArraySize(); ++n) if (yReference_.linearArray()[n] < minVal) minVal = yReference_.linearArray()[n];
	return minVal;
}

// Return maximum of reference y values
double DataSpaceRange::referenceYMax()
{
	if (yReference_.linearArraySize() == 0) return 0.0;
	
	double maxVal = yReference_.linearArray()[0];
	for (int n=1; n<yReference_.linearArraySize(); ++n) if (yReference_.linearArray()[n] > maxVal) maxVal = yReference_.linearArray()[n];
	return maxVal;
}

// Return calculated y value specified
double DataSpaceRange::calculatedY(int xIndex, int zIndex)
{
	return yCalculated_.at(xIndex, zIndex);
}

// Copy values from stored source collection, using index data provided
bool DataSpaceRange::copyValues(IndexData xIndex, IndexData zIndex)
{
	// Grab source abscissa values and display data array
	int nAbscissaPoints = parent_.sourceCollection()->displayAbscissa().nItems();
	List<DisplayDataSet>& dataSets = parent_.sourceCollection()->displayData();
	DisplayDataSet* dataSet;
	int actualZ, actualX;

	yReference_ = 0.0;

	// Loop over z indices defined in range
	for (int z = 0; z < nDataSets_; ++z)
	{
		// Grab the dataSet that we want, taking into account the zIndex definition
		if (zIndex.type() == IndexData::NormalIndex) dataSet = dataSets[z+displayDataSetStart_];
		else if (zIndex.type() == IndexData::RelativeIndex)
		{
			// Check current index, accounting for offset defined in zIndex
			actualZ = displayDataSetStart_+z+zIndex.offset();
			if ((actualZ < 0) || (actualZ >= dataSets.nItems()))
			{
				Messenger::warn("Relative offset defined for Z reference when copying values, and %i is out of range (available indices are 1 through %i).\n", actualZ+1, dataSets.nItems());
				dataSet = NULL;
			}
			else dataSet = dataSets[actualZ];
		}
		else if (zIndex.type() == IndexData::FixedIndex)
		{
			// Check current index, accounting for offset defined in zIndex
			actualZ = zIndex.index();
			if ((actualZ < 0) || (actualZ >= dataSets.nItems()))
			{
				Messenger::warn("Absolute index defined for Z reference when copying values, and %i is out of range (available indices are 1 through %i).\n", actualZ+1, dataSets.nItems());
				dataSet = NULL;
			}
			else dataSet = dataSets[actualZ];
		}

		// Check validity of dataset - if none is set, zero the relevant values_ entry and move on
		if (!dataSet) continue;

		// Grab data from dataSet
		const Array<double>& yRef = dataSet->y();

		// Loop over x indices defined in range
		if (xIndex.type() == IndexData::FixedIndex) actualX = xIndex.index();
		for (int x = 0; x<nPoints_; ++x)
		{
			// Calculate actual X value to use
			if (xIndex.type() == IndexData::NormalIndex) actualX = abscissaStart_+x;
			else if (xIndex.type() == IndexData::RelativeIndex) actualX = abscissaStart_+x+xIndex.offset();

			// Check index of x against valid abscissa range
			if ((x >= 0) && (x < nAbscissaPoints)) yReference_.at(x, z) = yRef.value(actualX);
		}
	}

	return true;
}

// Calculate values from specified equation
bool DataSpaceRange::calculateValues(Expression& equation, Variable* xVariable, Variable* zVariable, const RefList<ReferenceVariable,bool>& usedReferences)
{
	bool success;

	// Need the yTypes_ array here, so check to see if there is anything in it...
	if (yTypes_.linearArraySize() == 0)
	{
		Messenger::print("Internal Error: yTypes_ array is empty in DataSpaceRange::calculateValues().\n");
		return false;
	}

	// Loop over datasets (z points)
	for (int n=0; n<nDataSets_; ++n)
	{
		// Set z value
		zVariable->set(z_.value(n));

		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yTypes_.at(i,n) == DisplayDataSet::NoPoint) continue;

			// Set x variable value
			xVariable->set(x_.value(i));

			// Generate reference values
			for (RefListItem<ReferenceVariable,bool>* ri = usedReferences.first(); ri != NULL; ri = ri->next) ri->item->updateValue(i, n);

			// Calculate and store y value
			yCalculated_.at(i,n) = equation.execute(success);
			if (!success) return false;
		}
	}

	return true;
}

// Return sos error between stored and reference values
double DataSpaceRange::sosError()
{
	double sos = 0.0, yDiff;

	// Loop over datasets (z values)
	for (int n=0; n<nDataSets_; ++n)
	{
		// Loop over abscissa values
		for (int i=0; i<nPoints_; ++i)
		{
			// Nothing to do if this point does not exist...
			if (yTypes_.at(i,n) == DisplayDataSet::NoPoint) continue;
			
			yDiff = yReference_.at(i,n) - yCalculated_.at(i,n);

			sos += yDiff * yDiff;
		}
	}

	return sos;
}

// Add calculated data into specified Collection
void DataSpaceRange::addCalculatedValues(Collection* target)
{
	// Target collection should already have had DataSet 'space' created in it
	for (int n=0; n<nDataSets_; ++n)
	{
		// Grab DataSet pointer
		DataSet* dataSet = target->dataSet(n+displayDataSetStart_);
		if (!dataSet)
		{
			Messenger::print("Internal Error: Couldn't retrieve dataset index %i from target collection.\n", n);
			return;
		}

		// Loop over x values
		for (int i=0; i<nPoints_; ++i)
		{
			dataSet->setX(i+abscissaStart_, x_.value(i));
			dataSet->setY(i+abscissaStart_, yCalculated_.at(i,n));
		}
	}
}

// Add / set fitted variable value
void DataSpaceRange::setFittedValue(const char* name, double fittedValue)
{
	NamedValue* value = NULL;
	for (value = fittedValues_.first(); value != NULL; value = value->next) if (value->name() == name) break;
	if (value == NULL)
	{
		value = fittedValues_.add();
		value->setName(name);
	}
	value->setValue(fittedValue);
}

// Return whether named fitted value exists
NamedValue* DataSpaceRange::hasFittedValue(const char* name)
{
	for (NamedValue* value = fittedValues_.first(); value != NULL; value = value->next) if (DissolveSys::sameString(value->name(),name)) return value;
	return NULL;
}

// Return value of named fitted value
double DataSpaceRange::fittedValue(const char* name)
{
	for (NamedValue* value = fittedValues_.first(); value != NULL; value = value->next) if (DissolveSys::sameString(value->name(),name)) return value->value();
	return 0.0;
}

// Return first in list of fitted values
NamedValue* DataSpaceRange::fittedValues()
{
	return fittedValues_.first();
}
