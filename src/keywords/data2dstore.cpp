// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/data2dstore.h"
#include "base/lineparser.h"

Data2DStoreKeyword::Data2DStoreKeyword(Data2DStore &data2DStore)
    : KeywordData<Data2DStore &>(KeywordBase::Data2DStoreData, data2DStore)
{
}

Data2DStoreKeyword::~Data2DStoreKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data2DStoreKeyword::minArguments() const
{
    // Must have reference data name and format as a minimum
    return 2;
}

// Return maximum number of arguments accepted
int Data2DStoreKeyword::maxArguments() const
{
    // Filename, name of data, and other args
    return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool Data2DStoreKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool Data2DStoreKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    // Loop over list of one-dimensional data
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
