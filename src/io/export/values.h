// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class ProcessPool;

// Value Export Formats
class ValueExportFileFormat : public FileAndFormat
{
    public:
    // Value Formats
    enum class ValueExportFormat
    {
        Simple
    };
    explicit ValueExportFileFormat(std::string_view filename = "", ValueExportFormat format = ValueExportFormat::Simple);
    ~ValueExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Return enum options for ValueExportFormat
    EnumOptions<ValueExportFileFormat::ValueExportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Export Functions
     */
    private:
    // Export simple value data using specified parser
    bool exportSimple(LineParser &parser, const std::vector<double> &data);

    public:
    // Export value data using current filename and format
    bool exportData(const std::vector<double> &data, LineParser &currentParser, ProcessPool *procPool = nullptr);
};
