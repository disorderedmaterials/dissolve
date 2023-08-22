// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class ProcessPool;
class DataBase;

// Data Import Formats
class DataImportFileFormat : public FileAndFormat
{
    public:
    DataImportFileFormat(EnumOptionsBase &formats, std::string_view filename, int format)
        : FileAndFormat(formats, filename, format){};
    ~DataImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    protected:
    // Set up keywords for the format
    virtual void setUpKeywords() = 0;

    /*
     * Import Functions
     */
    public:
    // Import Data1D using current filename and format
    bool importData(DataBase &data, const ProcessPool *procPool = nullptr) { return false; };
    // Import Data1D using supplied parser and current format
    bool importData(LineParser &parser, DataBase &data) { return false; };
};
