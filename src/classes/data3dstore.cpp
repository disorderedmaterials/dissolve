// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
