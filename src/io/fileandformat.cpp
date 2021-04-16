// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/fileandformat.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

FileAndFormat::FileAndFormat(int format) { format_ = format; }

FileAndFormat::FileAndFormat(std::string_view filename, int format) : format_(format), filename_{filename} {}

FileAndFormat::operator std::string_view() const { return filename_; }

/*
 * Formats
 */

// Convert text string to format index
int FileAndFormat::format(std::string_view fmtString) const
{
    for (auto n = 0; n < nFormats(); ++n)
        if (DissolveSys::sameString(fmtString, formatKeyword(n)))
            return n;

    return nFormats();
}

// Set format index
void FileAndFormat::setFormatIndex(int id) { format_ = id; }

// Return format index
int FileAndFormat::formatIndex() const { return format_; }

// Return format string
std::string FileAndFormat::format() const
{
    if ((format_ < 0) || (format_ >= nFormats()))
        return "???";
    else
        return formatKeyword(format_);
}

// Return nice format string
std::string FileAndFormat::description() const
{
    if ((format_ < 0) || (format_ >= nFormats()))
        return "???";
    else
        return formatDescription(format_);
}

// Print available formats
void FileAndFormat::printAvailableFormats() const
{
    for (auto n = 0; n < nFormats(); ++n)
        Messenger::print("  {:12}  {}\n", formatKeyword(n), formatDescription(n));
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

// Return whether a filename and format have been set
bool FileAndFormat::hasValidFileAndFormat() const
{
    if (filename_.empty())
        return false;
    if ((format_ < 0) || (format_ >= nFormats()))
        return false;

    return true;
}

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
    // Convert first argument to format type
    format_ = format(parser.argsv(startArg));
    if (format_ == nFormats())
    {
        Messenger::print("Unrecognised format '{}' given for file. Recognised formats are:\n\n", parser.argsv(startArg));

        printAvailableFormats();

        return false;
    }

    // Set filename if present
    if (parser.hasArg(startArg + 1))
    {
        filename_ = parser.argsv(startArg + 1);

        // Check that the file exists?
        if (fileMustExist() && (!DissolveSys::fileExists(filename_)))
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

        // Do we recognise the keyword?
        KeywordBase *keyword = keywords_.find(parser.argsv(0));
        if (!keyword)
            return Messenger::error("Unrecognised option '{}' found in file and format block.\n", parser.argsv(0));

        // Read in the keyword's data
        if (!keyword->read(parser, 1, coreData))
            return Messenger::error("Error reading option '{}'.\n", keyword->name());
    }

    return true;
}

// Write format / filename to specified parser
bool FileAndFormat::writeFilenameAndFormat(LineParser &parser, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, formatKeyword(format_), filename_);
}

// Write options and end block
bool FileAndFormat::writeBlock(LineParser &parser, std::string_view prefix) const
{
    return keywords_.write(parser, fmt::format("{}  ", prefix));
}
