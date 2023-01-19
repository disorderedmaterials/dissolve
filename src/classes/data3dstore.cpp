// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/data3dstore.h"
#include "base/lineparser.h"
#include "io/import/data3d.h"

/*
 * Data
 */

// Add named data reference to store, reading file and format from specified parser / starting argument
bool Data3DStore::addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                          const CoreData &coreData)
{
    // Create new data
    auto &[data, format] = *data_.emplace_back(std::make_shared<std::pair<Data3D, Data3DImportFileFormat>>()).get();
    data.setTag(dataName);

    // Read the file / format
    if (!format.read(parser, startArg, endKeyword, coreData))
        return false;

    // Load the data
    return format.importData(data, parser.processPool());
}

// Check to see if the named data is present in the store
bool Data3DStore::containsData(std::string_view name) const
{
    return std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data->first.tag() == name; }) != data_.end();
}

// Return named data
OptionalReferenceWrapper<const Data3D> Data3DStore::data(std::string_view name) const
{
    auto it = std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data->first.tag() == name; });
    if (it == data_.end())
        return {};

    return (*it)->first;
}

// Return vector of all data
const std::vector<std::shared_ptr<std::pair<Data3D, Data3DImportFileFormat>>> &Data3DStore::data() const { return data_; }
