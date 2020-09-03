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

#pragma once

#include "io/import/data3d.h"
#include "math/data3d.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
/* none */

// Data3D Store
class Data3DStore
{
    public:
    Data3DStore();
    ~Data3DStore();

    /*
     * Data
     */
    private:
    // List of contained data
    List<Data3D> data_;
    // References for Data3D and associated file/format
    RefDataList<Data3D, Data3DImportFileFormat> dataReferences_;

    public:
    // Add named data reference to store, reading file and format from specified parser / starting argument
    bool addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword, CoreData &coreData);
    // Check to see if the named data is present in the store
    bool containsData(std::string_view name) const;
    // Return named data
    const Data3D &data(std::string_view name) const;
    // Return list of all data
    const List<Data3D> &data() const;
    // Return list of all data references
    const RefDataList<Data3D, Data3DImportFileFormat> &dataReferences() const;
};
