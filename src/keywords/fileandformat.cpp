// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/fileandformat.h"
#include "base/lineparser.h"
#include "io/fileandformat.h"

FileAndFormatKeyword::FileAndFormatKeyword(FileAndFormat &fileAndFormat, std::string_view endKeyword)
    : KeywordData<FileAndFormat &>(KeywordBase::FileAndFormatData, fileAndFormat), endKeyword_{endKeyword}
{
}

FileAndFormatKeyword::~FileAndFormatKeyword() {}

/*
 * Keyword Options
 */

// Return whether the underlying FileAndFormat has any options
bool FileAndFormatKeyword::hasOptions() const { return (data_.keywords().keywords().nItems() > 0); }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int FileAndFormatKeyword::minArguments() const
{
    // Must have data format as bare minimum
    return 1;
}

// Return maximum number of arguments accepted
int FileAndFormatKeyword::maxArguments() const
{
    // Data format and filename
    return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool FileAndFormatKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!data_.read(parser, startArg, endKeyword_, coreData))
        return Messenger::error("Failed to read file/format.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool FileAndFormatKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_.writeFilenameAndFormat(parser, fmt::format("{}{}  ", prefix, keywordName)))
        return false;
    if (!data_.writeBlock(parser, prefix))
        return false;
    if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
        return false;

    return true;
}
