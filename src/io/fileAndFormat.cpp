// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/fileAndFormat.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"

FileAndFormat::FileAndFormat(EnumOptionsBase &formats, std::string_view filename, std::optional<int> formatIndex)
    : formats_(formats), formatIndex_(formatIndex), filename_{filename}
{
}

/*
 * Formats
 */

// Return formats enum as the base object
const EnumOptionsBase &FileAndFormat::formats() const { return formats_; }

// Set current format by index
void FileAndFormat::setFormatByIndex(int index)
{
    assert(index >= 0 && index < formats_.nOptions());

    formatIndex_ = index;
}

// Return current format index
int FileAndFormat::formatIndex() const
{
    assert(formatIndex_);
    return *formatIndex_;
}

// Return current format keyword
std::string FileAndFormat::formatKeyword() const { return formatIndex_ ? formats_.keywordByIndex(*formatIndex_) : "???"; }

// Return current format description
std::string FileAndFormat::formatDescription() const
{
    return formatIndex_ ? formats_.descriptionByIndex(*formatIndex_) : "???";
}

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
KeywordStore &FileAndFormat::keywords() { return keywords_; }

/*
 * Read / Write
 */

// Read format / filename from specified parser
FileAndFormat::ReadResult FileAndFormat::read(LineParser &parser, int startArg, std::string_view endKeyword,
                                              const CoreData &coreData)
{
    // Convert first argument to format index
    auto formatId = formats_.keywordIndex(parser.argsv(startArg));
    if (!formatId)
    {
        Messenger::error("Unrecognised format '{}' given for file. Recognised formats are:\n\n", parser.argsv(startArg));

        printAvailableFormats();

        return ReadResult::UnrecognisedFormat;
    }
    formatIndex_ = formatId.value();

    // Set filename
    filename_ = parser.argsv(startArg + 1);

    // Parse any additional options until we find the end of the block
    while (!parser.eofOrBlank())
    {
        // Read the next line
        if (parser.getArgsDelim() != LineParser::Success)
            return ReadResult::GeneralReadError;

        // Is this the end of the block?
        if (DissolveSys::sameString(parser.argsv(0), endKeyword))
            break;

        // Can we parse the keyword?
        auto result = keywords_.deserialise(parser, coreData);
        if (result == KeywordBase::ParseResult::Unrecognised)
        {
            Messenger::error("Unrecognised option '{}' found in file and format block.\n", parser.argsv(0));
            return ReadResult::UnrecognisedOption;
        }
        else if (result == KeywordBase::ParseResult::Failed)
        {
            Messenger::error("Error reading option '{}'.\n", parser.argsv(0));
            return ReadResult::GeneralReadError;
        }
    }

    // Check that the file exists?
    if (fileMustExist() && (filename_ != "@") && (!DissolveSys::fileExists(filename_)))
    {
        Messenger::warn("Specified file '{}' does not exist.\n", filename_);
        return ReadResult::FileNotFound;
    }

    return ReadResult::Success;
}

// Write format / filename to specified parser
bool FileAndFormat::writeFilenameAndFormat(LineParser &parser, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'\n", prefix, formatIndex_ ? formats_.keywordByIndex(*formatIndex_) : "???", filename_);
}

// Write options and end block
bool FileAndFormat::writeBlock(LineParser &parser, std::string_view prefix) const
{
    return keywords_.serialise(parser, fmt::format("{}  ", prefix));
}

// Express as a serialisable value
SerialisedValue FileAndFormat::serialise() const
{
    SerialisedValue result = {{"filename", filename_},
                              {"format", formatIndex_ ? formats_.keywordByIndex(*formatIndex_) : "???"}};
    SerialisedValue keywords;
    keywords = keywords_.serialiseOnto(keywords);
    if (!keywords.is_uninitialized())
        result["keywords"] = keywords;
    return result;
}

// Read values from a serialisable value
void FileAndFormat::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    filename_ = toml::find<std::string>(node, "filename");
    formatIndex_ = formats_.keywordIndex(toml::find<std::string>(node, "format"));
    Serialisable::optionalOn(node, "keywords",
                             [this, &coreData](const auto node) { keywords_.deserialiseFrom(node, coreData); });
}
