/*
	*** Data Store
	*** src/classes/datastore.h
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

#ifndef DISSOLVE_DATASTORE_H
#define DISSOLVE_DATASTORE_H

#include "math/xydata.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// File Reference
class FileReference
{
	public:
	// Constructor
	FileReference(const char* filename = NULL, int xColumn = -1, int yColumn = -1, int errorColumn = -1);
	// Destructor
	~FileReference();


	/*
	 * Data
	 */
	private:
	// Source filename
	CharString filename_;
	// Column used for x values
	int xColumn_;
	// Column used for y values
	int yColumn_;
	// Column used for error values
	int errorColumn_;

	public:
	// Return source filename
	const char* filename() const;
	// Return column used for x values
	int xColumn() const;
	// Return column used for y values
	int yColumn() const;
	// Return column used for error values
	int errorColumn() const;
};

// Data Store
class DataStore
{
	public:
	// Constructor
	DataStore();
	// Destructor
	~DataStore();


	/*
	 * Data
	 */
	private:
	// List of contained one-dimensional data
	List<XYData> data1D_;
	// References for XYData and associated file information
	RefList<XYData,FileReference> data1DReferences_;

	public:
	// Load one-dimensional data into store
	bool addData1D(const char* filename, const char* name, int xColumn = 0, int yColumn = 1);
	// Load one-dimensional data into store using specified pool
	bool addData1D(ProcessPool& pool, const char* filename, const char* name, int xColumn = 0, int yColumn = 1);
	// Check to see if the named one-dimensional data is present in the store
	bool containsData1D(const char* name) const;
	// Return named one-dimensional data
	const XYData& data1D(const char* name) const;
	// Return list of all one-dimensional data
	const List<XYData>& data1D() const;
	// Return list of all one-dimensional data references
	const RefList<XYData,FileReference>& data1DReferences() const;
};

#endif
