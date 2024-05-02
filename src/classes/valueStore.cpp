// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/valueStore.h"
#include "base/lineParser.h"
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
    if (format.read(parser, startArg, endKeyword, coreData) != FileAndFormat::ReadResult::Success)
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

// Express as a serialisable value
SerialisedValue ValueStore::serialise() const
{
    SerialisedValue result = SerialisedValue::array_type{};
    for (auto &[tag, data, format] : data_)
        result.push_back({{"tag", tag}, {"values", data}, {"format", format}});
    return result;
}

// Read values from a serialisable value
void ValueStore::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this, &coreData](const auto &item)
             {
                 auto &[tag, data, format] = data_.emplace_back();
                 tag = toml::find<std::string>(item, "tag");
                 data = toml::find<std::vector<double>>(item, "values");
                 format.deserialise(item.at("format"), coreData);
             });
}
