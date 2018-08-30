/*
	*** DataSpace
	*** src/gui/uchroma/classes/dataspace.cpp
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

// Constructor
DataSpace::DataSpace()
{
	// Private variables
	sourceCollection_ = NULL;
	sourceCollectionDataUsedAt_ = -1;
	displayDataSetStart_ = -1;
	displayDataSetEnd_ = -1;
	nDataSets_ = 0;
	abscissaStart_ = -1;
	abscissaEnd_ = -1;
	nPoints_ = 0;
	global_ = false;
	orthogonal_ = false;
}

// Destructor
DataSpace::~DataSpace()
{
}

// Initialise ranges
bool DataSpace::initialise(Collection* sourceCollection, int xIndexMin, int xIndexMax, int zIndexMin, int zIndexMax, bool orthogonal, bool global)
{
	// Check source collection
	if (!Collection::objectValid(sourceCollection, "collection in DataSpace::initialise()")) return false;

	// We will only reinitialise everything if we really have to.
	// If only the x or z-ranges have changed, we can prune/extend accordingly 
	bool startFromScratch = false, resizeX = false, resizeZ = false;
	if (sourceCollection_ != sourceCollection) startFromScratch = true;
	else if (sourceCollectionDataUsedAt_ != sourceCollection_->dataVersion()) startFromScratch = true;
	else if (orthogonal_ != orthogonal) startFromScratch = true;
	else if (global_ != global) startFromScratch = true;
	else if ((xIndexMin != abscissaStart_) || (xIndexMax != abscissaEnd_)) resizeX = true;
	else if ((zIndexMin != displayDataSetStart_) || (zIndexMax != displayDataSetEnd_)) resizeZ = true;

	// Resize list?
	DataSpaceRange* range;
	if ((!startFromScratch) && (!global))
	{
		int deltaStart = 0, deltaEnd = 0;
		if ((!orthogonal) && resizeZ)
		{
			deltaStart = displayDataSetStart_ - zIndexMin;
			deltaEnd = zIndexMax - displayDataSetEnd_;
		}
		else if (orthogonal && resizeX)
		{
			deltaStart = abscissaStart_ - xIndexMin;
			deltaEnd = xIndexMax - abscissaEnd_;
		}
		for (int n = 0; n < deltaStart; ++n) { range = ranges_.add(*this); ranges_.moveToStart(range); }
		for (int n = deltaStart; n < 0; ++n) ranges_.removeFirst();
		for (int n = 0; n < deltaEnd; ++n) range = ranges_.add(*this);
		for (int n = deltaEnd; n < 0; ++n) ranges_.removeLast();
	}

	sourceCollection_ = sourceCollection;
	sourceCollectionDataUsedAt_ = sourceCollection_->dataVersion();
	orthogonal_ = orthogonal;
	global_ = global;
	const Array<double>& abscissa = sourceCollection_->displayAbscissa();

	// Store indices and check limits
	displayDataSetStart_ = zIndexMin;
	displayDataSetEnd_ = zIndexMax;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	if ((displayDataSetStart_ < 0) || (displayDataSetEnd_ >= sourceCollection_->nDataSets()) || (nDataSets_ < 1))
	{
		Messenger::error("Invalid dataset range used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", displayDataSetStart_, displayDataSetEnd_, sourceCollection_->nDataSets()-1); 
		return false;
	}
	abscissaStart_ = xIndexMin;
	abscissaEnd_ = xIndexMax;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;
	if ((abscissaStart_ < 0) || (abscissaEnd_ >= abscissa.nItems()) || (nPoints_ < 1))
	{
		Messenger::error("Invalid abscissa range used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", abscissaStart_, abscissaEnd_, abscissa.nItems()-1); 
		return false;
	}

	// Clear old ranges before adding new ones
	DataSpaceRange* currentRange = NULL;
	if (startFromScratch) ranges_.clear();
	else currentRange = ranges_.first();

	if (orthogonal_)
	{
		Messenger::print("Setting up orthogonal (ZY) data over %e < x < %e and %e < z < %e\n", abscissa.constAt(abscissaStart_), abscissa.constAt(abscissaEnd_), sourceCollection_->dataSet(displayDataSetStart_)->z(), sourceCollection_->dataSet(displayDataSetEnd_)->z());

		if (global_)
		{
			if (startFromScratch) currentRange = ranges_.add(*this);
			range->set(sourceCollection_, abscissaStart_, abscissaEnd_, displayDataSetStart_, displayDataSetEnd_, false);
			currentRange = currentRange->next;
		}
		else for (int n = abscissaStart_; n<= abscissaEnd_; ++n)
		{
			if (startFromScratch) currentRange = ranges_.add(*this);
			currentRange->set(sourceCollection_, n, n, displayDataSetStart_, displayDataSetEnd_, false);
			currentRange = currentRange->next;
		}
	}
	else
	{
		// Source data is normal XY slices from the current collection
		Messenger::print("Setting up normal (XY) data over %e < x < %e and %e < z < %e\n", abscissa.constAt(abscissaStart_), abscissa.constAt(abscissaEnd_), sourceCollection_->dataSet(displayDataSetStart_)->z(), sourceCollection_->dataSet(displayDataSetEnd_)->z());

		if (global_)
		{
			if (startFromScratch) currentRange = ranges_.add(*this);
			range->set(sourceCollection_, abscissaStart_, abscissaEnd_, displayDataSetStart_, displayDataSetEnd_, false);
			currentRange = currentRange->next;
		}
		else for (int n = displayDataSetStart_; n<= displayDataSetEnd_; ++n)
		{
			if (startFromScratch) currentRange = ranges_.add(*this);
			currentRange->set(sourceCollection_, abscissaStart_, abscissaEnd_, n, n, false);
			currentRange = currentRange->next;
		}
	}
	
	return true;
}

// Initialise data space, matching size in source DataSpace
bool DataSpace::initialise(const DataSpace& source, bool referenceDataOnly)
{
	sourceCollection_ = source.sourceCollection_;

	// Check for a valid source collection before we start...
	if (!Collection::objectValid(sourceCollection_, "source collection in DataSpace::initialise(DataSpace&)")) return false;

	// Store indices and check limits
	displayDataSetStart_ = source.displayDataSetStart_;
	displayDataSetEnd_ = source.displayDataSetEnd_;
	nDataSets_ = (displayDataSetEnd_ - displayDataSetStart_) + 1;
	if ((displayDataSetStart_ < 0) || (displayDataSetEnd_ >= sourceCollection_->nDataSets()) || (nDataSets_ < 1))
	{
		Messenger::error("Invalid dataset range (from existing DataSpace) used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", displayDataSetStart_, displayDataSetEnd_, sourceCollection_->nDataSets()-1); 
		return false;
	}
	const Array<double>& abscissa = sourceCollection_->displayAbscissa();
	abscissaStart_ = source.abscissaStart_;
	abscissaEnd_ = source.abscissaEnd_;
	nPoints_ = (abscissaEnd_ - abscissaStart_) + 1;
	if ((abscissaStart_ < 0) || (abscissaEnd_ >= abscissa.nItems()) || (nPoints_ < 1))
	{
		Messenger::error("Invalid abscissa range (from existing DataSpace) used to generate DataSpace data : %i < n < %i (available range= 0 to %i).\n", abscissaStart_, abscissaEnd_, abscissa.nItems()-1); 
		return false;
	}

	ranges_.clear();
	for (DataSpaceRange* range = source.ranges_.first(); range != NULL; range = range->next)
	{
		DataSpaceRange* newRange = ranges_.add(*this);
		newRange->set(sourceCollection_, range, referenceDataOnly);
	}

	return true;
}

// Return number of ranges in list
int DataSpace::nDataSpaceRanges()
{
	return ranges_.nItems(); 
}

// Return source collection
Collection* DataSpace::sourceCollection()
{
	return sourceCollection_;
}

// Return range list
DataSpaceRange* DataSpace::dataSpaceRanges()
{
	return ranges_.first();
}

// Return range specified
DataSpaceRange* DataSpace::dataSpaceRange(int index)
{
	return ranges_[index];
}

// Copy calculated y data to destination collection specified
void DataSpace::copy(Collection* destinationCollection)
{
	// Check for valid source and destination collections before we start...
	if (!Collection::objectValid(sourceCollection_, "source collection in DataSpace::copy()")) return;
	if (!Collection::objectValid(destinationCollection, "destination collection in DataSpace::copy()")) return;

	// Clear any existing datasets in the destination collection
	destinationCollection->clearDataSets();

	// Create destination datasets using those in the sourceCollection_ to get the z values, and size the x/y arrays
	for (int n=displayDataSetStart_; n<=displayDataSetEnd_; ++n)
	{
		DataSet* originalDataSet = sourceCollection_->dataSet(n);
		DataSet* newDataSet = destinationCollection->addDataSet();
		newDataSet->initialiseData(nPoints_);
		destinationCollection->setDataSetZ(newDataSet, originalDataSet->z());
		newDataSet->setName(CharString("Fit to: %s", originalDataSet->name()));
	}

	// Copy data from DataSpaceRanges/DataSpaceData into the new datasets
	for (DataSpaceRange* fitRange = ranges_.first(); fitRange != NULL; fitRange = fitRange->next) fitRange->addCalculatedValues(destinationCollection);
}
