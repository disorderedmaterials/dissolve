/*
	*** XYData Store
	*** src/classes/xydatastore.cpp
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

#include "classes/xydatastore.h"

// Constructor
XYDataStore::XYDataStore()
{
}

// Destructor
XYDataStore::~XYDataStore()
{
}

/*
 * Data
 */

// Add specified data
bool XYDataStore::add(const char* filename, const char* name, int xColumn, int yColumn)
{
	XYData* data = data_.add();
	data->setName(name);

	return data->load(filename, xColumn, yColumn);
}

// Add data through specified pool
bool XYDataStore::add(ProcessPool& pool, const char* filename, const char* name, int xColumn, int yColumn)
{
	XYData* data = data_.add();
	data->setName(name);

	return data->load(pool, filename, xColumn, yColumn);
}

// Check to see if the named data is present in the store
bool XYDataStore::contains(const char* name) const
{
	ListIterator<XYData> dataIterator(data_);
	while (XYData* data = dataIterator.iterate()) if (DissolveSys::sameString(name, data->name())) return true;

	return false;
}

// Return named data
const XYData& XYDataStore::data(const char* name) const
{
	ListIterator<XYData> dataIterator(data_);
	while (XYData* xyData = dataIterator.iterate()) if (DissolveSys::sameString(name, xyData->name())) return (*xyData);

	static XYData dummy;
	Messenger::warn("Data named '%s' was requested from XYDataStore, but it does not exist. Return an empty XYData...\n", name);
	return dummy;
}

// Return list of all data
const List<XYData>& XYDataStore::data() const
{
	return data_;
}
