// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/fileandformat.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

FileAndFormat::FileAndFormat(EnumOptionsBase &formats, std::string_view filename) : formats_(formats), filename_{filename} {}

FileAndFormat::operator std::string_view() const { return filename_; }

/*
 * Formats
 */

// Return formats enum as the base object
EnumOptionsBase &FileAndFormat::formats() { return formats_; }

// Return current format keyword
std::string FileAndFormat::format() const { return formats_.keywordByIndex(formats_.index()); }

// Return current format description
std::string FileAndFormat::description() const { return formats_.descriptionByIndex(formats_.index()); }

// Print available formats
void FileAndFormat::printAvailableFormats() const
{
    for (auto n = 0; n < formats_.nOptions(); ++n)
        Messenger::print("  {:12}  {}\n", formats_.keywordByIndex(n), formats_.descriptionByIndex(n));
}

/*
 * Filename
 */

// Return whether the file actually exists
bool FileAndFormat::fileExists() const
{
    // If no filename is currently set, return false. Otherwise, check for it
    return (filename_.empty() ? false : DissolveSys::fileExists(filename_));
}

// Set filename / basename
void FileAndFormat::setFilename(std::string_view filename) { filename_ = filename; }

// Return filename / basename
std::string_view FileAndFormat::filename() const { return filename_; }

/*
 * Check
 */

// Return whether a filename has been set
bool FileAndFormat::hasFilename() const { return (!filename_.empty()); }

/*
 * Additional Options
 */

// Return available keywords
KeywordList &FileAndFormat::keywords() { return keywords_; }

/*
 * Read / Write
 */

// Read format / filename from specified parser
bool FileAndFormat::read(LineParser &parser, int startArg, std::string_view endKeyword, const CoreData &coreData)
{
    // Convert first argument to format index
    auto formatId = formats_.keywordIndex(parser.argsv(startArg));
    if (!formatId)
    {
        Messenger::print("Unrecognised format '{}' given for file. Recognised formats are:\n\n", parser.argsv(startArg));

        printAvailableFormats();

        return false;
    }
    formats_.setIndex(formatId.value());

    // Set filename if present
    if (parser.hasArg(startArg + 1))
    {
        filename_ = parser.argsv(startArg + 1);

        // Check that the file exists?
        if (fileMustExist() && (filename_ != "@") && (!DissolveSys::fileExists(filename_)))
            return Messenger::error("Specified file '{}' does not exist.\n", filename_);
    }

    // Parse any additional options until we find the end of the block
    while (!parser.eofOrBlank())
    {
        // Read the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Is this the end of the block?
        if (DissolveSys::sameString(parser.argsv(0), endKeyword))
            break;

        // Can we parse the keyword?
        auto result = keywords_.parse(parser, coreData);
        if (result == KeywordBase::Unrecognised)
            return Messenger::error("Unrecognised option '{}' found in file and format block.\n", parser.argsv(0));
        else if (result == KeywordBase::Failed)
            return Messenger::error("Error reading option '{}'.\n", parser.argsv(0));
    }

    return true;
}

// Write format / filename to specified parser
bool FileAndFormat::writeFilenameAndFormat(LineParser &parser, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, formats_.keywordByIndex(formats_.index()), filename_);
}

// Write options and end block
bool FileAndFormat::writeBlock(LineParser &parser, std::string_view prefix) const
{
    return keywords_.write(parser, fmt::format("{}  ", prefix));
}
