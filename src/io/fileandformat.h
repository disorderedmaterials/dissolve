// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/list.h"

// Forward Declarations
class CoreData;
class LineParser;

// File / Format Base
class FileAndFormat
{
    public:
    FileAndFormat(EnumOptionsBase &formats, std::string_view filename = "");
    virtual ~FileAndFormat() = default;
    operator std::string_view() const;

    /*
     * File and Format
     */
    protected:
    // Formats enum as the base object
    EnumOptionsBase &formats_;

    public:
    // Return formats enum as the base object
    EnumOptionsBase &formats();
    // Return current format keyword
    std::string format() const;
    // Return current format description
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
