// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class ProcessPool;

// Value Import Formats
class ValueImportFileFormat : public FileAndFormat
{
    public:
    // Value Formats
    enum class ValueImportFormat
    {
        Simple
    };
    explicit ValueImportFileFormat(std::string_view filename = "", ValueImportFormat format = ValueImportFormat::Simple);
    ~ValueImportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Return enum options for ValueImportFormat
    EnumOptions<ValueImportFileFormat::ValueImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Import Functions
     */
    private:
    // Import simple value data using specified parser
    bool importSimple(LineParser &parser, std::vector<double> &data);

    public:
    // Import value data using current filename and format
    bool importData(std::vector<double> &data, LineParser &currentParser, const ProcessPool *procPool = nullptr);
};
