// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/data3dstore.h"
#include "base/lineparser.h"
#include "classes/data3dstore.h"

Data3DStoreKeyword::Data3DStoreKeyword(Data3DStore &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
dissolve::any_ptr Data3DStoreKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data3DStoreKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> Data3DStoreKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Data3DStoreKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    set_ = true;

    return true;
}

// Serialise data to specified LineParser
bool Data3DStoreKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &[data, format] : data_.data())
    {
        if (!format.writeFilenameAndFormat(parser, fmt::format("{}{}  '{}'  ", prefix, keywordName, data.tag())))
            return false;
        if (!format.writeBlock(parser, fmt::format("{}  ", prefix)))
            return false;
        if (!parser.writeLineF("{}End{}\n", prefix, name()))
            return false;
    }

    return true;
}
