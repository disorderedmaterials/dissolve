// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/valuestore.h"
#include "base/lineparser.h"
#include "io/import/values.h"

/*
 * Data
 */

// Add named data reference to store, reading file and format from specified parser / starting argument
bool ValueStore::addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                         const CoreData &coreData)
{
    // Create new data
    auto &[tag, data, format] = data_.emplace_back();
    tag = dataName;

    // Read the file / format
    if (!format.read(parser, startArg, endKeyword, coreData))
        return false;

    // Load the data
    return format.importData(data, parser, parser.processPool());
}

// Check to see if the named data is present in the store
bool ValueStore::containsData(std::string_view name) const
{
    return std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return std::get<0>(data) == name; }) != data_.end();
}

// Return named data
OptionalReferenceWrapper<const std::vector<double>> ValueStore::data(std::string_view name) const
{
    auto it = std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return std::get<0>(data) == name; });
    if (it == data_.end())
        return {};

    return std::get<1>(*it);
}

// Return vector of all data
const std::list<std::tuple<std::string, std::vector<double>, ValueImportFileFormat>> &ValueStore::data() const { return data_; }
