/*
	*** Data3D Store
	*** src/classes/data3dstore.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_DATA3DSTORE_H
#define DISSOLVE_DATA3DSTORE_H

#include "math/data3d.h"
#include "io/import/data3d.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
/* none */

// Data3D Store
class Data3DStore
{
	public:
	// Constructor
	Data3DStore();
	// Destructor
	~Data3DStore();


	/*
	 * Data
	 */
	private:
	// List of contained data
	List<Data3D> data_;
	// References for Data3D and associated file/format
	RefDataList<Data3D,Data3DImportFileFormat> dataReferences_;

	public:
	// Add named data reference to store, reading file and format from specified parser / starting argument
	bool addData(const char* dataName, LineParser& parser, int startArg, const char* endKeyword, const CoreData& coreData);
	// Check to see if the named data is present in the store
	bool containsData(const char* name) const;
	// Return named data
	const Data3D& data(const char* name) const;
	// Return list of all data
	const List<Data3D>& data() const;
	// Return list of all data references
	const RefDataList<Data3D,Data3DImportFileFormat>& dataReferences() const;
};

#endif
