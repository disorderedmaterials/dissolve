/*
    *** File/Format Base Class
    *** src/io/fileandformat.h
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

#pragma once

#include "keywords/list.h"
#include "templates/list.h"

// Forward Declarations
class CoreData;
class LineParser;

// File / Format Base
class FileAndFormat
{
    public:
    FileAndFormat(int format = 0);
    FileAndFormat(std::string_view filename = "", int format = 0);
    virtual ~FileAndFormat() = default;
    operator std::string_view() const;

    /*
     * Available Formats
     */
    protected:
    // Format of associated file
    int format_;

    public:
    // Return number of available formats
    virtual int nFormats() const = 0;
    // Return format keyword for supplied index
    virtual std::string_view formatKeyword(int id) const = 0;
    // Return description string for supplied index
    virtual std::string_view formatDescription(int id) const = 0;
    // Convert text string to format index
    int format(std::string_view fmtString) const;
    // Set format index
    void setFormatIndex(int id);
    // Return format index
    int formatIndex() const;
    // Return format string
    std::string_view format() const;
    // Return description string
    std::string_view description() const;
    // Print available formats
    void printAvailableFormats() const;

    /*
     * Filename / Basename
     */
    protected:
    // Associated filename / basename
    std::string filename_;

    public:
    // Return whether the file must exist
    virtual bool fileMustExist() const = 0;
    // Return whether the file actually exists
    bool fileExists() const;
    // Set filename / basename
    void setFilename(std::string_view filename);
    // Return filename / basename
    std::string_view filename() const;

    /*
     * Check
     */
    public:
    // Return whether a filename has been set
    bool hasFilename() const;
    // Return whether a filename and format have been set
    bool hasValidFileAndFormat() const;

    /*
     * Additional Options
     */
    protected:
    // Available keywords options
    KeywordList keywords_;

    public:
    // Return available keywords
    KeywordList &keywords();

    /*
     * Read / Write
     */
    public:
    // Read format / filename from specified parser
    bool read(LineParser &parser, int startArg, std::string_view endKeyword, CoreData &coreData);
    // Write format / filename to specified parser
    bool writeFilenameAndFormat(LineParser &parser, std::string_view prefix);
    // Write options and end block
    bool writeBlock(LineParser &parser, std::string_view prefix);
};
