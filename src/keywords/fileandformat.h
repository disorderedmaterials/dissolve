// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/data.h"

// Forward Declarations
class FileAndFormat;

// Keyword with a file format
class FileAndFormatKeyword : public KeywordData<FileAndFormat &>
{
    public:
    FileAndFormatKeyword(FileAndFormat &fileAndFormat, std::string_view endKeyword);
    ~FileAndFormatKeyword();

    /*
     * End Keyword
     */
    private:
    // Keyword that signals the end of 'block' for the data
    std::string endKeyword_;

    /*
     * Keyword Options
     */
    public:
    // Return whether the underlying FileAndFormat has any options
    bool hasOptions() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    int maxArguments() const override;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const;
};
