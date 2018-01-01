/*
	*** DataSpace
	*** src/gui/uchroma/classes/dataspace.h
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

#ifndef DUQ_UCHROMADATASPACE_H
#define DUQ_UCHROMADATASPACE_H

#include "gui/uchroma/classes/dataspacerange.h"
#include "templates/list.h"

// Forward Declarations
class Collection;

/*
 * DataSpace
 */
class DataSpace
{
	public:
	// Constructor
	DataSpace();
	// Destructor
	~DataSpace();


	/*
	 * Data
	 */
	private:
	// Source collection
	Collection* sourceCollection_;
	// Data version of source collection when it was used to construct data
	int sourceCollectionDataUsedAt_;
	// Range of datasets spanned by this instance
	int displayDataSetStart_, displayDataSetEnd_;
	// Total number of sequential DisplayDataSet covered
	int nDataSets_;
	// Range of datasets spanned by this instance
	int abscissaStart_, abscissaEnd_;
	// Total number of sequential abscissa values covered
	int nPoints_;
	// Whether the fit data is orthogonal
	bool orthogonal_;
	// Whether the fit data is global
	bool global_;
	// List of individual fitting targets / slices / ranges
	ParentList<DataSpaceRange,DataSpace> ranges_;

	public:
	// Initialise data space
	bool initialise(Collection* sourceCollection, int xIndexMin, int xIndexMax, int zIndexMin, int zIndexMax, bool orthogonal, bool global);
	// Initialise data space, matching size in source DataSpace
	bool initialise(const DataSpace& source, bool referenceDataOnly);
	// Return source collection
	Collection* sourceCollection();
	// Return number of ranges in list
	int nDataSpaceRanges();
	// Return range list
	DataSpaceRange* dataSpaceRanges();
	// Return range specified
	DataSpaceRange* dataSpaceRange(int index);
	// Copy calculated y data to destination collection specified
	void copy(Collection* destinationCollection);
};

#endif
