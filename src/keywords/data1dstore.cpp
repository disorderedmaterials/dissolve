// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/data1dstore.h"
#include "base/lineparser.h"
#include "classes/data1dstore.h"
#include "io/import/data1d.h"

Data1DStoreKeyword::Data1DStoreKeyword(Data1DStore &data1DStore)
    : KeywordData<Data1DStore &>(KeywordBase::Data1DStoreData, data1DStore)
{
}

Data1DStoreKeyword::~Data1DStoreKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data1DStoreKeyword::minArguments() const
{
    // Must have reference data name and format as a minimum
    return 2;
}

// Return maximum number of arguments accepted
int Data1DStoreKeyword::maxArguments() const
{
    // Filename, name of data, and other args
    return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool Data1DStoreKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool Data1DStoreKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
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
