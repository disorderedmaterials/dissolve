// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "keywords/store.h"

// Forward Declarations
class CoreData;
class LineParser;

// File / Format Base
class FileAndFormat : public Serialisable<const CoreData &>
{
    public:
    FileAndFormat(EnumOptionsBase &formats, std::string_view filename = "", std::optional<int> formatIndex = {});
    FileAndFormat(const FileAndFormat &source) = default;
    virtual ~FileAndFormat() = default;

    /*
     * Format
     */
    protected:
    // Formats enum as the base object
    EnumOptionsBase &formats_;
    // Index of current format
    std::optional<int> formatIndex_;

    public:
    // Return formats enum as the base object
    const EnumOptionsBase &formats() const;
    // Set current format by index
    void setFormatByIndex(int index);
    // Return current format index
    int formatIndex() const;
    // Return current format keyword
    std::string formatKeyword() const;
    // Return current format description
    std::string formatDescription() const;
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
    KeywordStore keywords_;

    public:
    // Return available keywords
    KeywordStore &keywords();

    /*
     * Read / Write
     */
    public:
    // Read result
    enum class ReadResult
    {
        UnrecognisedFormat,
        FileNotFound,
        UnrecognisedOption,
        GeneralReadError,
        Success
    };
    // Read format / filename from specified parser
    ReadResult read(LineParser &parser, int startArg, std::string_view endKeyword, const CoreData &coreData);
    // Write format / filename to specified parser
    bool writeFilenameAndFormat(LineParser &parser, std::string_view prefix) const;
    // Write options and end block
    bool writeBlock(LineParser &parser, std::string_view prefix) const;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
