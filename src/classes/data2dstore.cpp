// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/data2dstore.h"
#include "base/lineparser.h"
#include "io/import/data2d.h"

/*
 * Data
 */

// Add named data reference to store, reading file and format from specified parser / starting argument
bool Data2DStore::addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                          const CoreData &coreData)
{
    // Create new data
    auto &data = data_.emplace_back();
    data.first.setTag(dataName);

    // Read the file / format
    if (!data.second.read(parser, startArg, endKeyword, coreData))
        return false;

    // Load the data
    return data.second.importData(data.first, parser.processPool());
}

// Check to see if the named data is present in the store
bool Data2DStore::containsData(std::string_view name) const
{
    return std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data.first.tag() == name; }) != data_.end();
}

// Return named data
OptionalReferenceWrapper<const Data2D> Data2DStore::data(std::string_view name) const
{
    auto it = std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data.first.tag() == name; });
    if (it == data_.end())
        return {};

    return it->first;
}

// Return vector of all data
const std::list<std::pair<Data2D, Data2DImportFileFormat>> &Data2DStore::data() const { return data_; }
