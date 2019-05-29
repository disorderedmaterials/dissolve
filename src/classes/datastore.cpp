/*
	*** Data Store
	*** src/classes/datastore.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/datastore.h"

/*
 * File Reference
 */

// Constructor
FileReference::FileReference(const char* filename, int xColumn, int yColumn, int errorColumn)
{
	filename_ = filename;
	xColumn_ = xColumn;
	yColumn_ = yColumn;
	errorColumn_ = errorColumn;
}

// Destructor
FileReference::~FileReference()
{
}

// Return source filename
const char* FileReference::filename() const
{
	return filename_.get();
}

// Return column used for x values
int FileReference::xColumn() const
{
	return xColumn_;
}

// Return column used for y values
int FileReference::yColumn() const
{
	return yColumn_;
}

/*
 * Data Store
 */

// Constructor
DataStore::DataStore()
{
}

// Destructor
DataStore::~DataStore()
{
}

// Load one-dimensional data into store
bool DataStore::addData1D(const char* filename, const char* name, int xColumn, int yColumn)
{
	// Create new data
	Data1D* data = data1D_.add();
	data->setName(name);

	// Add reference
	data1DReferences_.add(data, FileReference(filename, xColumn, yColumn));

	return data->load(filename, xColumn, yColumn);
}

// Load one-dimensional data into store using specified pool
bool DataStore::addData1D(ProcessPool& pool, const char* filename, const char* name, int xColumn, int yColumn)
{
	// Create new data
	Data1D* data = data1D_.add();
	data->setName(name);

	// Add reference
	data1DReferences_.add(data, FileReference(filename, xColumn, yColumn));

	return data->load(pool, filename, xColumn, yColumn);
}

// Check to see if the named one-dimensional data is present in the store
bool DataStore::containsData1D(const char* name) const
{
	ListIterator<Data1D> dataIterator(data1D_);
	while (Data1D* data = dataIterator.iterate()) if (DissolveSys::sameString(name, data->name())) return true;

	return false;
}

// Return named one-dimensional data
const Data1D& DataStore::data1D(const char* name) const
{
	ListIterator<Data1D> dataIterator(data1D_);
	while (Data1D* xyData = dataIterator.iterate()) if (DissolveSys::sameString(name, xyData->name())) return (*xyData);

	static Data1D dummy;
	Messenger::warn("Data named '%s' was requested from DataStore, but it does not exist. Return an empty Data1D...\n", name);
	return dummy;
}

// Return list of all one-dimensional data
const List<Data1D>& DataStore::data1D() const
{
	return data1D_;
}

// Return list of all one-dimensional data references
const RefList<Data1D,FileReference>& DataStore::data1DReferences() const
{
	return data1DReferences_;
}
