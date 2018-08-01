/*
	*** XYData Store
	*** src/classes/xydatastore.h
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

#ifndef DISSOLVE_XYDATASTORE_H
#define DISSOLVE_XYDATASTORE_H

#include "math/xydata.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// XYData Store
class XYDataStore
{
	public:
	// Constructor
	XYDataStore();
	// Destructor
	~XYDataStore();


	/*
	 * Data
	 */
	private:
	// List of contained data
	List<XYData> data_;

	public:
	// Add specified data to store
	bool add(const char* filename, const char* name, int xColumn = 0, int yColumn = 1);
	// Add data through specified pool
	bool add(ProcessPool& pool, const char* filename, const char* name, int xColumn = 0, int yColumn = 1);
	// Check to see if the named data is present in the store
	bool contains(const char* name) const;
	// Return named data
	const XYData& data(const char* name) const;
	// Return list of all data
	const List<XYData>& data() const;
};

#endif
