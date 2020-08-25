/*
    *** Keyword - File and Format
    *** src/keywords/fileandformat.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
bool FileAndFormatKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (!data_.read(parser, startArg, endKeyword_, coreData))
        return Messenger::error("Failed to read file/format.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool FileAndFormatKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    if (!data_.writeFilenameAndFormat(parser, fmt::format("{}{}  ", prefix, keywordName)))
        return false;
    if (!data_.writeBlock(parser, prefix))
        return false;
    if (!parser.writeLineF("{}End{}\n", prefix, keywordName))
        return false;

    return true;
}
