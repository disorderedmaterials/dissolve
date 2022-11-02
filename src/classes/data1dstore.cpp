// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/data1dstore.h"
#include "base/lineparser.h"
#include "io/import/data1d.h"
#include <algorithm>

/*
 * Data
 */

// Add named data reference to store, reading file and format from specified parser / starting argument
bool Data1DStore::addData(std::string_view dataName, LineParser &parser, int startArg, std::string_view endKeyword,
                          const CoreData &coreData)
{
    // Create new data
    auto &[data, format] = *data_.emplace_back(std::make_shared<std::pair<Data1D, Data1DImportFileFormat>>()).get();
    data.setTag(dataName);

    // Read the file / format
    if (!format.read(parser, startArg, endKeyword, coreData))
        return false;

    // Load the data
    return format.importData(data, parser.processPool());
}

// Check to see if the named data is present in the store
bool Data1DStore::containsData(std::string_view name) const
{
    return std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data->first.tag() == name; }) != data_.end();
}

// Return named data
OptionalReferenceWrapper<const Data1D> Data1DStore::data(std::string_view name) const
{
    auto it = std::find_if(data_.begin(), data_.end(), [&name](auto &data) { return data->first.tag() == name; });
    if (it == data_.end())
        return {};

    return (*it)->first;
}

// Return vector of all data
const std::vector<std::shared_ptr<std::pair<Data1D, Data1DImportFileFormat>>> &Data1DStore::data() const { return data_; }
SerialisedValue Data1DStore::serialise() const
{
    if (data_.empty())
        return {};
    SerialisedValue result = toml::array{};
    std::transform(data_.begin(), data_.end(), std::back_inserter(result), [](const auto &item) -> SerialisedValue {
        return {{"data", item->first}, {"format", item->second}};
    });
    return result;
}
void Data1DStore::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    auto &arr = node.as_array();
    std::transform(arr.begin(), arr.end(), std::back_inserter(data_), [&coreData](const auto &item) {
        auto pair = std::make_shared<std::pair<Data1D, Data1DImportFileFormat>>();
        pair->first.deserialise(item.at("data"));
        pair->second.deserialise(item.at("format"), coreData);
        return pair;
    });
}
