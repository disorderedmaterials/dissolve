// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/fileandformat.h"
#include "base/lineparser.h"
#include "io/fileandformat.h"

FileAndFormatKeyword::FileAndFormatKeyword(FileAndFormat &data, std::string_view endKeyword)
    : KeywordBase(typeid(this)), data_(data), endKeyword_{endKeyword}
{
}

/*
 * Data
 */

// Return reference to data
FileAndFormat &FileAndFormatKeyword::data() { return data_; }
const FileAndFormat &FileAndFormatKeyword::data() const { return data_; }

/*
 * Keyword Options
 */

// Return whether the underlying FileAndFormat has any options
bool FileAndFormatKeyword::hasOptions() const { return !data_.keywords().keywords().empty(); }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> FileAndFormatKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool FileAndFormatKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    auto result = data_.read(parser, startArg, endKeyword_, coreData);

    // Fail on hard errors
    if (result == FileAndFormat::ReadResult::UnrecognisedFormat || result == FileAndFormat::ReadResult::UnrecognisedOption)
        return Messenger::error("Failed to read file/format for '{}'.\n", name());

    return true;
}

// Serialise data to specified LineParser
bool FileAndFormatKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_.hasFilename())
        return true;

    if (!data_.writeFilenameAndFormat(parser, fmt::format("{}{}  ", prefix, keywordName)))
        return false;
    if (!data_.writeBlock(parser, prefix))
        return false;
    if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
        return false;

    return true;
}
