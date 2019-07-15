/*
	*** Data3D Store
	*** src/classes/data3dstore.cpp
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

#include "classes/data3dstore.h"
#include "modules/import/import.h"
#include "base/lineparser.h"

// Constructor
Data3DStore::Data3DStore()
{
}

// Destructor
Data3DStore::~Data3DStore()
{
}

// Load data into store
bool Data3DStore::addData(Data3DImportFileFormat fileAndFormat, const char* name)
{
	// Create new data
	Data3D* data = data_.add();
	data->setName(name);

	// Add reference
	dataReferences_.add(data, fileAndFormat);

	// Open file and check that we're OK to proceed reading from it
	LineParser parser;
	if ((!parser.openInput(fileAndFormat.filename())) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for reading.\n", fileAndFormat.filename());

	// Load the data
	return ImportModule::readData3D(fileAndFormat.data3DFormat(), parser, *data);
}

// Load data into store using specified pool
bool Data3DStore::addData(ProcessPool& pool, Data3DImportFileFormat fileAndFormat, const char* name)
{
	// Create new data
	Data3D* data = data_.add();
	data->setName(name);

	// Add reference
	dataReferences_.add(data, fileAndFormat);

	// Open file and check that we're OK to proceed reading from it
	LineParser parser(&pool);
	if ((!parser.openInput(fileAndFormat.filename())) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for reading.\n", fileAndFormat.filename());

	// Load the data
	return ImportModule::readData3D(fileAndFormat.data3DFormat(), parser, *data);
}

// Check to see if the named data is present in the store
bool Data3DStore::containsData(const char* name) const
{
	ListIterator<Data3D> dataIterator(data_);
	while (Data3D* data = dataIterator.iterate()) if (DissolveSys::sameString(name, data->name())) return true;

	return false;
}

// Return named data
const Data3D& Data3DStore::data(const char* name) const
{
	ListIterator<Data3D> dataIterator(data_);
	while (Data3D* xyData = dataIterator.iterate()) if (DissolveSys::sameString(name, xyData->name())) return (*xyData);

	static Data3D dummy;
	Messenger::warn("Data named '%s' was requested from Data3DStore, but it does not exist. Returning an empty Data3D...\n", name);
	return dummy;
}

// Return list of all data
const List<Data3D>& Data3DStore::data() const
{
	return data_;
}

// Return list of all data references
const RefList<Data3D,Data3DImportFileFormat>& Data3DStore::dataReferences() const
{
	return dataReferences_;
}
