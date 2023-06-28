// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/data2dstore.h"
#include "base/lineparser.h"
#include "classes/data2dstore.h"

Data2DStoreKeyword::Data2DStoreKeyword(Data2DStore &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
Data2DStore &Data2DStoreKeyword::data() { return data_; }
const Data2DStore &Data2DStoreKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data2DStoreKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> Data2DStoreKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Data2DStoreKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    return true;
}

// Serialise data to specified LineParser
bool Data2DStoreKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
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