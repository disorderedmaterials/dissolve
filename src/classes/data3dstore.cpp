/*
    *** Data3D Store
    *** src/classes/data3dstore.cpp
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

#include "classes/data3dstore.h"
#include "base/lineparser.h"
#include "io/import/data3d.h"

Data3DStore::Data3DStore() {}

Data3DStore::~Data3DStore() {}

/*
 * Data
 */

// Add named data reference to store, reading file and format from specified parser / starting argument
bool Data3DStore::addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                          CoreData &coreData)
{
    // Create new data
    Data3D *data = data_.add();
    data->setName(dataName);

    // Add reference to data
    RefDataItem<Data3D, Data3DImportFileFormat> *ref = dataReferences_.append(data);

    // Read the file / format
    if (!ref->data().read(parser, startArg, endKeyword, coreData))
        return false;

    // Load the data
    return ref->data().importData(*data, parser.processPool());
}

// Check to see if the named data is present in the store
bool Data3DStore::containsData(std::string_view name) const
{
    ListIterator<Data3D> dataIterator(data_);
    while (Data3D *data = dataIterator.iterate())
        if (DissolveSys::sameString(name, data->name()))
            return true;

    return false;
}

// Return named data
const Data3D &Data3DStore::data(std::string_view name) const
{
    ListIterator<Data3D> dataIterator(data_);
    while (Data3D *xyData = dataIterator.iterate())
        if (DissolveSys::sameString(name, xyData->name()))
            return (*xyData);

    static Data3D dummy;
    Messenger::warn("Data named '{}' was requested from Data3DStore, but it does not exist. Returning an empty Data3D...\n",
                    name);
    return dummy;
}

// Return list of all data
const List<Data3D> &Data3DStore::data() const { return data_; }

// Return list of all data references
const RefDataList<Data3D, Data3DImportFileFormat> &Data3DStore::dataReferences() const { return dataReferences_; }
