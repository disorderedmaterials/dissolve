// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    virtual std::string formatKeyword(int id) const = 0;
    // Return description string for supplied index
    virtual std::string formatDescription(int id) const = 0;
    // Convert text string to format index
    int format(std::string_view fmtString) const;
    // Set format index
    void setFormatIndex(int id);
    // Return format index
    int formatIndex() const;
    // Return format string
    std::string format() const;
    // Return description string
    std::string description() const;
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
    bool read(LineParser &parser, int startArg, std::string_view endKeyword, const CoreData &coreData);
    // Write format / filename to specified parser
    bool writeFilenameAndFormat(LineParser &parser, std::string_view prefix) const;
    // Write options and end block
    bool writeBlock(LineParser &parser, std::string_view prefix) const;
};
