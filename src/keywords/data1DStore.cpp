// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/data1DStore.h"
#include "base/lineParser.h"
#include "classes/data1DStore.h"
#include "io/import/data1D.h"

Data1DStoreKeyword::Data1DStoreKeyword(Data1DStore &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
Data1DStore &Data1DStoreKeyword::data() { return data_; }
const Data1DStore &Data1DStoreKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data1DStoreKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> Data1DStoreKeyword::maxArguments() const
{
    // Filename, name of data, and other args
    return std::nullopt;
}

// Deserialise from supplied LineParser, starting at given argument offset
bool Data1DStoreKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    return true;
}

// Serialise data to specified LineParser
bool Data1DStoreKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &sharedDataPointer : data_.data())
    {
        auto &[data, format] = *sharedDataPointer.get();
        if (!format.writeFilenameAndFormat(parser, fmt::format("{}{}  '{}'  ", prefix, keywordName, data.tag())))
            return false;
        if (!format.writeBlock(parser, fmt::format("{}  ", prefix)))
            return false;
        if (!parser.writeLineF("{}End{}\n", prefix, name()))
            return false;
    }

    return true;
}

// Express as a serialisable value
SerialisedValue Data1DStoreKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void Data1DStoreKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.deserialise(node, coreData);
}

// Has not changed from initial value
bool Data1DStoreKeyword::isDefault() const { return data_.data().empty(); }