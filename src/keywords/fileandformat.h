// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"

// Forward Declarations
class FileAndFormat;

// Keyword with a file format
class FileAndFormatKeyword : public KeywordBase
{
    public:
    FileAndFormatKeyword(FileAndFormat &fileAndFormat, std::string_view endKeyword);
    ~FileAndFormatKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to data
    FileAndFormat &data_;
    // Keyword that signals the end of 'block' for the data
    std::string endKeyword_;

    public:
    // Return reference to data
    FileAndFormat &data();
    const FileAndFormat &data() const;

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
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
};
