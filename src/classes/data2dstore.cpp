/*
	*** Data2D Store
	*** src/classes/data2dstore.cpp
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

#include "classes/data2dstore.h"
#include "io/import/data2d.h"
#include "base/lineparser.h"

// Constructor
Data2DStore::Data2DStore()
{
}

// Destructor
Data2DStore::~Data2DStore()
{
}

// Load data into store
bool Data2DStore::addData(Data2DImportFileFormat fileAndFormat, const char* name)
{
	// Create new data
	Data2D* data = data_.add();
	data->setName(name);

	// Add reference
	dataReferences_.append(data, fileAndFormat);

	// Load the data
	return fileAndFormat.importData(*data);
}

// Load data into store using specified pool
bool Data2DStore::addData(ProcessPool* pool, Data2DImportFileFormat fileAndFormat, const char* name)
{
	// Create new data
	Data2D* data = data_.add();
	data->setName(name);

	// Add reference
	dataReferences_.append(data, fileAndFormat);

	// Load the data
	return fileAndFormat.importData(*data, pool);
}

// Check to see if the named data is present in the store
bool Data2DStore::containsData(const char* name) const
{
	ListIterator<Data2D> dataIterator(data_);
	while (Data2D* data = dataIterator.iterate()) if (DissolveSys::sameString(name, data->name())) return true;

	return false;
}

// Return named data
const Data2D& Data2DStore::data(const char* name) const
{
	ListIterator<Data2D> dataIterator(data_);
	while (Data2D* xyData = dataIterator.iterate()) if (DissolveSys::sameString(name, xyData->name())) return (*xyData);

	static Data2D dummy;
	Messenger::warn("Data named '%s' was requested from Data2DStore, but it does not exist. Returning an empty Data2D...\n", name);
	return dummy;
}

// Return list of all data
const List<Data2D>& Data2DStore::data() const
{
	return data_;
}

// Return list of all data references
const RefDataList<Data2D,Data2DImportFileFormat>& Data2DStore::dataReferences() const
{
	return dataReferences_;
}
