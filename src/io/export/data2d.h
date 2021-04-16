// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"
#include "templates/optionalref.h"

// Forward Declarations
class Data2DBase;

// Data2D Export Formats
class Data2DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum Data2DExportFormat
    {
        BlockData2D,
        CartesianData2D,
        nData2DExportFormats
    };
    Data2DExportFileFormat(std::string_view filename = "", Data2DExportFormat format = Data2DExportFileFormat::BlockData2D);

    /*
     * Format Access
     */
    private:
    // Return enum options for Data2DExportFormat
    static EnumOptions<Data2DExportFileFormat::Data2DExportFormat> data2DExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as Data2DExportFormat
    Data2DExportFormat data2DFormat() const;

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
    // Export Data2D as simple block data
    bool exportBlock(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                     const Array2D<double> &values, OptionalReferenceWrapper<const Array2D<double>> errors = {});
    // Export Data2D as cartesian data
    bool exportCartesian(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                         const Array2D<double> &values, OptionalReferenceWrapper<const Array2D<double>> errors = {});

    public:
    // Export Data2D using current filename and format
    bool exportData(const Data2DBase &data);
};
