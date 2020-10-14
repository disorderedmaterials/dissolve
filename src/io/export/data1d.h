// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Data1D;

// Data1D Export Formats
class Data1DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum Data1DExportFormat
    {
        XYData1D,
        nData1DExportFormats
    };
    Data1DExportFileFormat(std::string_view filename = "", Data1DExportFormat format = Data1DExportFileFormat::XYData1D);

    /*
     * Format Access
     */
    private:
    // Return enum options for Data1DExportFormat
    static EnumOptions<Data1DExportFileFormat::Data1DExportFormat> &data1DExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as Data1DExportFormat
    Data1DExportFormat data1DFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return false; }

    /*
     * Export Functions
     */
    private:
    // Export Data1D as simple XY (or XYE) data
    bool exportXY(LineParser &parser, const Data1D &data);

    public:
    // Export Data1D using current filename and format
    bool exportData(const Data1D &data);
};
